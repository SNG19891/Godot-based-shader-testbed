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

#include "show-degree.h"
#include "glwidget.h"

void ShowDegree::onPluginLoad() 
{
	float sum = 0; // sum of the degrees
 	Scene* scn = scene();
    	// per cada objecte
    	//for (unsigned int i=0; i<scene->objects().size(); ++i)    
    	{
        	const Object& obj = scn->objects()[0];
        	// per cada cara
        	for(unsigned int c=0; c<obj.faces().size(); c++)
        	{
			const Face& face = obj.faces()[c];
			sum += face.numVertices();
			/*
            		
            		// per cada vertex
            		for(int v=0; v<face.numVertices(); v++)
            		{ 
                		int index = face.vertexIndex(v);
            		}
			*/
        	}
		degree = sum / obj.vertices().size();
	}		
}


void ShowDegree::postFrame() 
{ 
	glColor3f(0.0, 0.0, 0.0);
	int x = 5;
	int y = 15;
	glwidget()->renderText(x,y, QString::number(degree));
}

