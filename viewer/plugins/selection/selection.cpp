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

#include "selection.h"
#include "glwidget.h"

void encodeID(int id, GLubyte* color)
{
    color[0] = id % 256;
    id /= 256;
    color[1] = id % 256;
    id /= 256;
    color[2] = id % 256;
    id /= 256;
    color[3] = id % 256;
}

int decodeID(GLubyte* color)
{
    return color[0]+(color[1]+(color[2]+color[3]*256)*256)*256;
}

void Selection::mouseReleaseEvent( QMouseEvent *e)
{
    if (! (e->button() & Qt::RightButton)) return;
    if ( e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier)) return;
    if (! (e->modifiers() & Qt::ControlModifier)) return;
    
    cout << "Selecting!" << endl;

    // 1. Dibujar la escena en modo seleccion
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    //camera()->setOpenGLProjectionMatrix();
    //camera()->setOpenGLModelviewMatrix();
    
    // per cada objecte
    for (unsigned int i=0; i<scene()->objects().size(); ++i)    
    {
        GLubyte color[4];
        encodeID(i,color);
        glColor4ubv(color);
        const Object& obj = scene()->objects()[i];
        // per cada cara
        for(unsigned int c=0; c<obj.faces().size(); c++)
        {
            const Face& face = obj.faces()[c];
            glBegin (GL_POLYGON);
            glNormal3f(face.normal().x(), face.normal().y(), face.normal().z());
            // per cada vertex
            for(int v=0; v<face.numVertices(); v++)
            { 
                const Point& p = obj.vertices()[face.vertexIndex(v)].coord();
                glVertex3f(p.x(), p.y(), p.z());
            }
            glEnd();
        }
    }
    
    glEnable(GL_LIGHTING);

    // 2. Read color from color buffer
    int x = (e->x());
    int y = glwidget()->height()-(e->y());

    GLubyte read[4];
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, read);
    //if (abits==0) 
    read[3]=0;
    int id = decodeID(read); 
    
    if (id<0 || id >= int(scene()->objects().size())) id=-1;
    
    // 3. Set selected object 
    scene()->setSelectedObject(id);
    cout << id << endl;
    
    // 4. Ask update
    glwidget()->update();
}

