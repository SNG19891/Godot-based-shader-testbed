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

#include "splatting.h"
#include <QFileDialog>
 
void Splatting::onPluginLoad()
{
	// VS
    QString vs_src = "uniform float radius; void main() {	gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex; 	gl_FrontColor  = vec4(normalize(gl_NormalMatrix*gl_Normal).z); 	gl_TexCoord[0] = (2.0/radius)*(gl_Vertex.xyzw+gl_Vertex.zxyw); }";
    vs = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vs->compileSourceCode(vs_src);

	// FS
    QString fs_src = "uniform sampler2D rock; uniform sampler2D grass; uniform sampler2D noise; void main() { vec4 ro = texture2D(rock, gl_TexCoord[0].st);             vec4 gr = texture2D(grass, gl_TexCoord[0].st);             float no = texture2D(noise, gl_TexCoord[0].st).r;             gl_FragColor =  gl_Color*mix(ro, gr, no); }";

    fs = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fs->compileSourceCode(fs_src);

	// Program  
  	program = new QOpenGLShaderProgram(this);
	program->addShader(vs);
	program->addShader(fs);
	program->link();

	// Load Texture 1
	glActiveTexture(GL_TEXTURE0);
	QString filename = QFileDialog::getOpenFileName(0, "Open Image (rock)", "/assig/grau-g/Textures", "Image file (*.png *.jpg)");	
	QImage im0(filename);	
	glGenTextures( 1, &textureId0);
	glBindTexture(GL_TEXTURE_2D, textureId0);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, im0.width(), im0.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, im0.bits());
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glBindTexture(GL_TEXTURE_2D, 0);

	// Load Texture 2
	glActiveTexture(GL_TEXTURE1);
	QString filename2 = QFileDialog::getOpenFileName(0, "Open Image (grass)", "/assig/grau-g/Textures", "Image file (*.png *.jpg)");	
	QImage im1(filename2);	
	glGenTextures( 1, &textureId1);
	glBindTexture(GL_TEXTURE_2D, textureId1);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, im1.width(), im1.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, im1.bits());
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glBindTexture(GL_TEXTURE_2D, 0);

	// Load Texture 3
	glActiveTexture(GL_TEXTURE2);
	QString filename3 = QFileDialog::getOpenFileName(0, "Open Image (noise)", "/assig/grau-g/Textures", "Image file (*.png *.jpg)");	
	QImage im2(filename3);	
	glGenTextures( 1, &textureId2);
	glBindTexture(GL_TEXTURE_2D, textureId2);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, im2.width(), im2.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, im2.bits());
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Splatting::preFrame() 
{
    // bind shader and define uniforms
    program->bind();
    program->setUniformValue("rock", 0);  // texture unit del primer sampler 
    program->setUniformValue("grass", 1);  // texture unit del segon  sampler
    program->setUniformValue("noise", 2);  // texture unit del tercer  sampler  
    program->setUniformValue("radius", float(scene()->boundingBox().radius()));  // radi d'una esfera que engloba l'escena
    // bind textures
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId0);
	glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureId1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureId2);
}

void Splatting::postFrame() 
{
    // unbind shader
    program->release();
   	// unbind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
}

