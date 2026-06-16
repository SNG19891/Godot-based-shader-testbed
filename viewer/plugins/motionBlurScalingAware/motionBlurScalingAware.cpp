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

#include "motionBlurScalingAware.h"
#include <QCoreApplication>
#include <QGuiApplication>

const int IMAGE_SIZE = 512;
float scale = 1.0;

void MotionBlur::onPluginLoad()
{
    //** 
    // option 1: This one yields the largest possible in the display, 
    //           rounded to larger integer.
    QCoreApplication * coreApp = QCoreApplication::instance();
    QGuiApplication * myApp =
          dynamic_cast<QGuiApplication*>(coreApp);
    if (not myApp) { cout << "Did not get a QGuiApplication pointer!" << endl;}
    else {
        scale = myApp->devicePixelRatio();
        cout << "Scale Upper Bound: " << scale << endl;
    }
    GLWidget &g = *glwidget();	
    g.makeCurrent();
    //**
    // option 2: This one yields current ratio
/*    scale = glwidget()->devicePixelRatio();
    cout << endl << "Scale: " << scale << endl << endl;
*/
    // Carregar shader, compile & link 
    vs = new QOpenGLShader(QOpenGLShader::Vertex, this);
    QString vsCode = 	"#version 330 core\n"
			"in vec3 vertex;"
			"void main(){"
			" gl_Position    = vec4(vertex,1.0);"
                        "}";
    vs->compileSourceCode(vsCode);
    cout << "VS log:" << vs->log().toStdString() << endl;


    fs = new QOpenGLShader(QOpenGLShader::Fragment, this);
    QString fsCode =    "#version 330 core\n"
			"out vec4 fragColor;"
			"uniform sampler2D colorMap;"
			"uniform float SIZE = 512;"
			"uniform float alpha;"			
			"void main(){"
			"   vec2 st = gl_FragCoord.xy / SIZE;"
			"   fragColor = vec4(texture(colorMap, st).rgb, alpha);"
                        "}";
    fs->compileSourceCode(fsCode);
    cout << "FS log:" << fs ->log().toStdString() << endl;

    program = new QOpenGLShaderProgram(this);
    program->addShader(vs);
    program->addShader(fs);
    program->link();
    cout << "Link log:" << program->log().toStdString() << endl;

    // uniforms
    program->bind();
    program->setUniformValue("colorMap", 0);
    program->setUniformValue("SIZE", float(IMAGE_SIZE*scale));
    //    program->release();

    // Setup texture for current view
    g.glActiveTexture(GL_TEXTURE0);

    g.glGenTextures( 1, &textureIdCur);
    g.glBindTexture(GL_TEXTURE_2D, textureIdCur);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    g.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_SIZE*scale, IMAGE_SIZE*scale, 0, GL_RGB, GL_FLOAT, NULL);
    g.glBindTexture(GL_TEXTURE_2D, 0);

   // Setup texture for previous view
    g.glActiveTexture(GL_TEXTURE0);
    g.glGenTextures( 1, &textureIdPrev);
    g.glBindTexture(GL_TEXTURE_2D, textureIdPrev);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    g.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    g.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_SIZE*scale, IMAGE_SIZE*scale, 0, GL_RGB, GL_FLOAT, NULL);
    g.glBindTexture(GL_TEXTURE_2D, 0);

    // Resize to power-of-two viewport size
    // g.resize(IMAGE_SIZE,IMAGE_SIZE);

    // Clear
    g.glClearColor(1,1,1,0);
    g.glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}


void MotionBlur::drawRect()
{
    GLWidget &g = *glwidget();	
    static bool created = false;
    static GLuint VAO_rect;

    // 1. Create VBO Buffers
    if (!created)
    {
        created = true;
        
        // Create & bind empty VAO
        g.glGenVertexArrays(1, &VAO_rect);
        g.glBindVertexArray(VAO_rect);

        // Create VBO with (x,y,z) coordinates
        float coords[] = { -1, -1, 0, 
                            1, -1, 0, 
                           -1,  1, 0, 
                            1,  1, 0};

        GLuint VBO_coords;
        g.glGenBuffers(1, &VBO_coords);
        g.glBindBuffer(GL_ARRAY_BUFFER, VBO_coords);
        g.glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);
        g.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        g.glEnableVertexAttribArray(0);
        g.glBindVertexArray(0);
    }

    // 2. Draw
    g.glBindVertexArray (VAO_rect);
    g.glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    g.glBindVertexArray(0);
}

void MotionBlur::onSceneClear() {
    GLWidget &g = *glwidget();	
    g.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}



bool MotionBlur::paintGL()
{
//    cout << "entered  paintGL()" << endl;
    GLWidget &g = *glwidget();	
    g.resize(IMAGE_SIZE,IMAGE_SIZE);
    //**
    // Option 2: can be refreshed here:
/*    scale = g.devicePixelRatio();
    cout << endl << "dynscale -> " << scale << endl;
*/
    // Clear
    g.glClearColor(1,1,1,0);
    g.glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Pass A. Draw scene (depth test enabled, alpha blending disabled)
    g.glEnable(GL_DEPTH_TEST);
    g.glDisable(GL_BLEND);
    if (drawPlugin()) drawPlugin()->drawScene();

    // Copy color buffer to textureIdCur 
    g.glBindTexture(GL_TEXTURE_2D, textureIdCur);
    g.glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, IMAGE_SIZE*scale, IMAGE_SIZE*scale);
    g.glBindTexture(GL_TEXTURE_2D, 0);	

    // Clear
    g.glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    g.glDisable(GL_DEPTH_TEST);    
    g.glEnable(GL_BLEND);
    g.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Pass B. Draw quad using texture from prev frame (depth test disabled, alpha blending enabled)
    g.glBindTexture(GL_TEXTURE_2D, textureIdPrev);
    program->bind();
    program->setUniformValue("SIZE", IMAGE_SIZE*scale);
    program->setUniformValue("alpha", float(1.0));  
    drawRect();
    program->release();
    g.glBindTexture(GL_TEXTURE_2D, 0);	

    // Pass C. Draw quad using texture from this frame (depth test disabled, alpha blending enabled)
    g.glBindTexture(GL_TEXTURE_2D, textureIdCur);
    program->bind();
    program->setUniformValue("SIZE", float(IMAGE_SIZE*scale));
    program->setUniformValue("alpha", float(0.05)); // POSEU VALOR ALPHA DE L'ENUNCIAT
    drawRect();
    program->release();
    g.glBindTexture(GL_TEXTURE_2D, 0);	

    // Copy color buffer to textureIdPrev (will be used for next frame)
    g.glBindTexture(GL_TEXTURE_2D, textureIdPrev);
    g.glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, IMAGE_SIZE*scale, IMAGE_SIZE*scale);
    g.glBindTexture(GL_TEXTURE_2D, 0);
    g.glDisable(GL_BLEND);
    g.glBlendFunc(GL_ONE, GL_ZERO);
    return true;
}


