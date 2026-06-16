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

#include "show-color.h"
#include "glwidget.h" 

void ShowColor::postFrame() 
{   
  // Escriu color del pixel sota el cursor
  int x = 15;
  int y = 30;
  QFont font;
  font.setPixelSize(32);

  GLWidget &g = *glwidget();
  g.makeCurrent();
  int mousex = g.mapFromGlobal(QCursor::pos()).x();
  int mousey = g.mapFromGlobal(QCursor::pos()).y();
  GLubyte c[3];
  g.glReadPixels(mousex, g.height()-mousey, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, c);
  painter.begin(&g);
  painter.setFont(font);
//   painter.setBackgroundMode(Qt::OpaqueMode);
  painter.drawText(x,y, QString("Color RGB: ")+QString::number(c[0])+QString(" ")+QString::number(c[1])+QString(" ")+QString::number(c[2]));

  // escriu num total vertexs duplicats
  // per cada objecte
  int count = 0;
  Scene* scn = scene();
  for (unsigned int i=0; i<scn->objects().size(); ++i)    
    {
      const Object& obj = scn->objects()[i];
      // per cada cara
      for(unsigned int c=0; c<obj.faces().size(); c++)
	{
	  const Face& face = obj.faces()[c];
	  count += face.numVertices();
	}
    }
  painter.drawText(x,y+35, QString("Num vertexs amb duplicats: ")+QString::number(count));
  painter.end();
  g.makeCurrent();
  g.glBlendFunc(GL_ONE, GL_ZERO);
}
