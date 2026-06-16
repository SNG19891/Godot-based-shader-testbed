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

#include "glwidget.h"
#include "inverter.h"
 
Inverter::~Inverter() {
}

void Inverter::onPluginLoad()
{
    // Carregar shader, compile & link 
	// Flashlight
    // QString fs_src = "const float r = 100.0; uniform int x; uniform int y; void main() {gl_FragColor=gl_Color*(1.0-length(gl_FragCoord.xy-vec2(x,y))/r);}";
	// Inverter
    QString fs_src = "#version 330 core\n"
    "const float r = 100.0;" 
    "uniform int x;" 
    "uniform int y;"
    "in vec3 frColor;"
    "out vec4 fragColor;" 
    "void main() {"
    "  float d = length(gl_FragCoord.xy-vec2(x,y));"
    "  if (d<100) fragColor=vec4(1)-vec4(frColor,0);"
    "  else fragColor = vec4(frColor,1);"
    "}";

    fs = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fs->compileSourceCode(fs_src);
    
    QString vs_src = "#version 330 core\n"
    "uniform mat4 modelViewProjectionMatrix;"
    "layout (location = 0) in vec3 vertex;"
    "layout (location = 2) in vec3 color;"
    "out vec3 frColor;"
    " "
    "void main() {"
    "   frColor = color;"
    "	gl_Position    = modelViewProjectionMatrix * vec4(vertex,1.0);"
    "}";
    QOpenGLShader vs(QOpenGLShader::Vertex, this);
    vs.compileSourceCode(vs_src);
       
    program = new QOpenGLShaderProgram(this);
    program->addShader(&vs);
    program->addShader(fs);
    program->link();
}

void Inverter::preFrame() 
{
    	// bind shader and define uniforms
    	program->bind();

	    int x = glwidget()->mapFromGlobal(QCursor::pos()).x();
	    int y = glwidget()->height() - glwidget()->mapFromGlobal(QCursor::pos()).y();
    	program->setUniformValue("x", x);
    	program->setUniformValue("y", y);
    	program->setUniformValue("modelViewProjectionMatrix", camera()->projectionMatrix()*
    	                                                      camera()->viewMatrix());

}

void Inverter::postFrame() 
{
    // unbind shader
    program->release();
}

