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

#include "textureDemo.h"
#include <QFileDialog>
 
void TextureDemo::onPluginLoad()
{
    GLWidget &g = *glwidget();
    g.makeCurrent();
	// VS
    QString vs_src = \
      "#version 330 core\n"
      "uniform mat4 modelViewProjectionMatrix;"
      "layout (location = 0) in vec3 vertex;"
      "layout (location = 3) in vec2 texCoord;"
      "out vec2 vtexCoord;"
      "void main() {"
      "  gl_Position = modelViewProjectionMatrix * vec4(vertex, 1.);"
      "  vtexCoord = texCoord;"
      "}";
    QOpenGLShader vs(QOpenGLShader::Vertex, this);
    vs.compileSourceCode(vs_src);
    
	// FS
    QString fs_src =
      "#version 330 core\n"
      "uniform sampler2D sampler0;"
      "uniform int repeat = 1;"
      "in vec2 vtexCoord;"
      "out vec4 fragColor;"
      "void main() {"
      "  fragColor = texture(sampler0, repeat * vtexCoord);"
      "}";
    QOpenGLShader fs(QOpenGLShader::Fragment, this); 
    fs.compileSourceCode(fs_src);
    
	// Program  
  	program = new QOpenGLShaderProgram(this);
    program->addShader(&vs);
	program->addShader(&fs);
	program->link();

	// Load Texture 1
    //  QString filename = QFileDialog::getOpenFileName(0, "Open Image", 
    //                     "/assig/grau-g/Textures", "Image file (*.png *.jpg)");	
    //  QImage img0(filename);
	QImage img0("/home/alvar/Docencia/G/NewViewer/Textures/checkerboard.png");
	QImage im0 = img0.convertToFormat(QImage::Format_ARGB32).rgbSwapped().mirrored();
    g.makeCurrent();
	g.glActiveTexture(GL_TEXTURE0);
	g.glGenTextures( 1, &textureId0);
	g.glBindTexture(GL_TEXTURE_2D, textureId0);
	g.glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, im0.width(), im0.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, im0.bits());
	g.glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureDemo::preFrame() 
{
    GLWidget &g = *glwidget();
    g.makeCurrent();
    
    // bind shader and define uniforms
    program->bind();
    program->setUniformValue("sampler0", 0);  // texture unit del primer sampler 
    program->setUniformValue("modelViewProjectionMatrix", 
                    g.camera()->projectionMatrix() * g.camera()->viewMatrix());
 
    // -----------------------------------------------------------------
    // modify repetition factor and texture parameters below:
    //
    program->setUniformValue("repeat", 1);  
    // bind textures
    g.glActiveTexture(GL_TEXTURE0);
    g.glBindTexture(GL_TEXTURE_2D, textureId0);
    // GLfloat color[] = {1., 1, 0., 0.};
    // g.glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color );
    // GL_REPEAT GL_MIRRORED_REPEAT GL_CLAMP_TO_EDGE GL_CLAMP_TO_BORDER
    GLenum wrapMode = GL_REPEAT;
	g.glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode );
	g.glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode );
    g.glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    g.glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void TextureDemo::postFrame() 
{
    GLWidget &g = *glwidget();
    g.makeCurrent();

    // bind default shaders
    g.defaultProgram()->bind();
    // unbind textures
    g.glActiveTexture(GL_TEXTURE0);
    g.glBindTexture(GL_TEXTURE_2D, 0);
}

