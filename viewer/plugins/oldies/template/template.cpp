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

#include "template.h"
#include <cstdlib>

// Documentation: http://www.cs.upc.edu/~virtual/Gdocs/interfaces/html/class_basic_plugin.html

void Template::onPluginLoad()
{
    // Load & compile VS 
    vs = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vs->compileSourceFile(glwidget()->getPluginPath()+"/../template/template.vert");
    qDebug() << vs->log() << endl;

    // Load & compile FS 
    fs = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fs->compileSourceFile(glwidget()->getPluginPath()+"/../template/template.frag");
    qDebug() << fs->log() << endl;

    // Link program
    program = new QOpenGLShaderProgram(this);
    program->addShader(vs);
    program->addShader(fs);
    program->link();
    qDebug() << program->log() << endl;

    // Timers
    connect(&timer, SIGNAL(timeout()), glwidget(), SLOT(updateGL()));
    timer.start();
    elapsedTimer.start();
}


void Template::onObjectAdd()
{
    unsigned int numObjects = scene()->objects().size();
    qDebug() << "Added new object " << endl;
    qDebug() << " Current scene has " << numObjects << " objects" << endl;
    unsigned int numFaces = scene()->objects()[numObjects-1].faces().size();
    unsigned int numVertices = scene()->objects()[numObjects-1].vertices().size();
    qDebug() << " Last object has " << numFaces << " faces and " << numVertices << " vertices" << endl;
}

void Template::preFrame()
{
}

void Template::postFrame()
{
    glColor3f(0.0, 0.0, 0.0);
	int x = 5;
	int y = 15;
	glwidget()->renderText(x,y, QString("[Template plugin] "));
    
}

bool Template::drawScene()
{
    // See example DrawImmediate and DrawVBO examples
    // If you draw the scene here, you should return true
    return false;
}
	
void Template::keyPressEvent (QKeyEvent *e)
{
    // if (e->key() == Qt::Key_C) ...
    qDebug() << "User pressed key " << e->key() << endl; 
    glwidget()->updateGL();
}

void Template::keyReleaseEvent (QKeyEvent *)
{}

void Template::mouseMoveEvent (QMouseEvent *) 
{}

void Template::mousePressEvent (QMouseEvent *e)
{   
    qDebug() << "Mouse (x,y) = " << e->x() << ", " << e->y() << endl;
}

void Template::mouseReleaseEvent (QMouseEvent *)
{}

void Template::wheelEvent (QWheelEvent *)
{}


bool Template::paintGL()
{
    // clear buffers
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    // setup matrices
    camera()->setOpenGLModelviewMatrix();
    camera()->setOpenGLProjectionMatrix();
    // draw axes
    glwidget()->drawAxes();
    // draw scene with shaders
    program->bind();
    program->setUniformValue("time", float(elapsedTimer.elapsed()/1000.));
    drawPlugin()->drawScene();
    program->release();

    return true;
}

