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

#include "reflection.h"

const int IMAGE_WIDTH = 1024;
const int IMAGE_HEIGHT = IMAGE_WIDTH;

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << endl;

int printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
			     file, line, gluErrorString(glErr), func);
        retCode = 1;
    }
    return retCode;
}

void Reflection::onPluginLoad()
{
    // Resize to power-of-two viewport
    glwidget()->resize(IMAGE_WIDTH,IMAGE_HEIGHT);

    // Carregar shader, compile & link 
    vs = new QGLShader(QGLShader::Vertex, this);
    vs->compileSourceFile(glwidget()->getPluginPath()+"/../reflection/reflection.vert");

    fs = new QGLShader(QGLShader::Fragment, this);
    fs->compileSourceFile(glwidget()->getPluginPath()+"/../reflection/reflection.frag");

    program = new QGLShaderProgram(this);
    program->addShader(vs);
    program->addShader(fs);
    program->link();

    // Setup texture
	glActiveTexture(GL_TEXTURE0);
	glGenTextures( 1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
}


bool Reflection::paintGL()
{
    // Pass 1. Draw scene in virtual pos
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    camera()->setOpenGLModelviewMatrix();

    glTranslatef(0,  scene()->boundingBox().min().y(), 0);
    glScalef(1, -1, 1);
    glTranslatef(0, -scene()->boundingBox().min().y(), 0);
    camera()->setOpenGLProjectionMatrix();
    glEnable(GL_LIGHTING);
    glColor3f(0.8, 0.8, 1.0);
    drawPlugin()->drawScene();

    // Get texture
    glBindTexture(GL_TEXTURE_2D, textureId);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glGenerateMipmap(GL_TEXTURE_2D);
    

    // Pass 2. Draw mirror as textured quad 
    camera()->setOpenGLModelviewMatrix();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    program->bind();
    program->setUniformValue("colorMap", 0);
    /*
    QMatrix4x4 biasMatrix;
    biasMatrix.translate(0.5, 0.5, 0.5);
    biasMatrix.scale(0.5, 0.5, 0.5);
    program->setUniformValue("mvpMatrix", biasMatrix*camera()->projectionMatrix()*camera()->modelviewMatrix());
    */

    Point min = scene()->boundingBox().min();
    float r = scene()->boundingBox().radius();
    glBegin(GL_POLYGON);
    glVertex3f(-r,min.y(), -r);
    glVertex3f( r,min.y(), -r);
    glVertex3f( r,min.y(),  r);
    glVertex3f(-r,min.y(),  r);        
    glEnd();
    
    program->release();
    glBindTexture(GL_TEXTURE_2D, 0);

    // Pass 3. Draw scene in real pos
    camera()->setOpenGLModelviewMatrix();
    drawPlugin()->drawScene();

    return true;
}

