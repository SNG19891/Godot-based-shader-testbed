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

#include "navigatedefault.h"
#include "glwidget.h"
#include "mousePositionMethod.h"
//#include <QMatrix4x4>
#define SPEED 1.2f

NavigateDefault::NavigateDefault() : pmouseAction(NONE)
{}

void NavigateDefault::mousePressEvent( QMouseEvent *e)
{
  pxClick = QROUND(e->POS().x());
  pyClick = QROUND(e->POS().y());

    if (e->button() & Qt::LeftButton && 
        ! (e->modifiers()&(Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
    {
        pmouseAction = ROTATE_EYE;
    }
    else if (e->button() & Qt::LeftButton &&  e->modifiers() & Qt::ShiftModifier)
    {
        pmouseAction = ZOOM;
    }
    else if (e->button() & Qt::RightButton && ! (e->modifiers()&(Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
    {
        pmouseAction = PAN;
    }
    else if (e->button() & Qt::LeftButton &&  e->modifiers() & Qt::ControlModifier)
    {
        pmouseAction = ROTATE;
    } 
}

void NavigateDefault::mouseReleaseEvent( QMouseEvent *)
{
    pmouseAction = NONE;
}

void NavigateDefault::mouseMoveEvent(QMouseEvent *e)
{ 
  float mouseDeltaX = QROUND(e->POS().x()) - pxClick;
  float mouseDeltaY = QROUND(e->POS().y()) - pyClick;

    Box box = glwidget()->boundingBoxIncludingAxes();
    //Point center = box.center();
    float radius = box.radius();

    if (pmouseAction == ROTATE)
    {
        camera()->incrementAngleY(-mouseDeltaX);  
        camera()->incrementAngleX(mouseDeltaY);
    }
    else if(pmouseAction == ROTATE_EYE)
    {
        // Rotate in view
        QMatrix4x4 current = camera()->viewMatrix();
        QMatrix4x4  rotation;
        rotation.setToIdentity();
        rotation.rotate(SPEED, mouseDeltaY, mouseDeltaX, 0);
        rotation = rotation * current;
        camera()->setRotation(rotation);
    }
    else if(pmouseAction == ZOOM) 
    {
        camera()->incrementDistance(mouseDeltaY * 0.01 * radius);
    }
    else if (pmouseAction == PAN)
    {
      double r = camera()->getDist()/2.;//scene()->boundingBox().radius();
        double x = -0.02 * mouseDeltaX * r;
        double y =  0.02 * mouseDeltaY * r;

        QMatrix4x4 m = camera()->viewMatrix();
        Vector s(m(0,0), m(0,1), m(0,2));      
        Vector u(m(1,0), m(1,1), m(1,2)); 
        camera()->pan(x * s + y * u);
    }

    // actualizar plans de retallat
    camera()->updateClippingPlanes(box);

    pxClick = QROUND(e->POS().x());
    pyClick = QROUND(e->POS().y());

    glwidget()->update();
}


