// GLarena, a plugin based platform to teach OpenGL programming
// © Copyright 2012-2018, ViRVIG Research Group, UPC, https://www.virvig.eu
// 
// This file is part of GLarena
//
// GLarena is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "vbos.h"
#include "glwidget.h"

VBOs::~VBOs()
{
	glDeleteBuffers(idsV.size(), &idsV[0]);
	glDeleteBuffers(idsN.size(), &idsN[0]);
    glDeleteBuffers(idsF.size(), &idsF[0]);
}


bool VBOs::drawScene()
{
    glColor3f(1.0f, 0.8f, 0.8f);
	for(unsigned int i=0; i<idsV.size(); i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, idsV[i]);
		glVertexPointer(3, GL_FLOAT, 0, (GLvoid*) 0);
		glEnableClientState(GL_VERTEX_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, idsN[i]);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*) 0);
		glEnableClientState(GL_NORMAL_ARRAY);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idsF[i]);
		glDrawElements(GL_TRIANGLES, numIndices[i], GL_UNSIGNED_INT, (GLvoid*) 0);
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

    return true;
}

void VBOs::onPluginLoad()
{
    for(unsigned int i=0; i<scene()->objects().size(); i++)
        addVBO(i);
}

void VBOs::onObjectAdd()
{
    addVBO(scene()->objects().size()-1);
}

void VBOs::addVBO(unsigned int objectID)
{  
    // build arrays of vertices, normals and indices; 
    // This version: 
    //  - each vertex/normal will appear n times (one for each triangle using it)
    //  - indices will be the trivial sequence: 0,1,2, 3,4,5, ...
    //  - non-triangular faces are triangulated on-the-fly (v0,v1,v2, v0,v2,v3, ...)
    unsigned int vIndex = 0;
    vector<float> vertices, normals;
	vector<int> faces;
	
    const Object& obj = scene()->objects()[objectID];
	for(unsigned int j=0; j<obj.faces().size(); j++)
	{
        const Face& face = obj.faces()[j];
	    Vector N = face.normal();
		for(unsigned int k=1; k<(unsigned int)face.numVertices()-1; k++)
		{
			Point P;
            P = obj.vertices()[face.vertexIndex(0)].coord();
			vertices.push_back(P.x()); vertices.push_back(P.y()); vertices.push_back(P.z());
			normals.push_back(N.x()); normals.push_back(N.y()); normals.push_back(N.z());

			P = obj.vertices()[face.vertexIndex(k)].coord();
			vertices.push_back(P.x()); vertices.push_back(P.y()); vertices.push_back(P.z());
			normals.push_back(N.x()); normals.push_back(N.y()); normals.push_back(N.z());

			P = obj.vertices()[face.vertexIndex(k+1)].coord();
			vertices.push_back(P.x()); vertices.push_back(P.y()); vertices.push_back(P.z());
			normals.push_back(N.x()); normals.push_back(N.y()); normals.push_back(N.z());

			faces.push_back(vIndex++); faces.push_back(vIndex++); faces.push_back(vIndex++);
		}
	}
    // get IDs & create VBO
    GLuint ids[3];
	glGenBuffers(3, &ids[0]);  
    idsV.push_back(ids[0]);
    idsN.push_back(ids[1]);
    idsF.push_back(ids[2]);

    unsigned int current = idsV.size()-1;
	glBindBuffer(GL_ARRAY_BUFFER, idsV[current]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, idsN[current]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*normals.size(), &normals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idsF[current]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*faces.size(), &faces[0], GL_STATIC_DRAW);
	numIndices.push_back(faces.size());
}

