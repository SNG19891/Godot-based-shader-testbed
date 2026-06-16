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

#include "morphing.h"
#include "glwidget.h"

void Morphing::onPluginLoad()
{
    glwidget()->makeCurrent();
    // Auto-update timer
    QTimer *utimer = new QTimer(this);
    connect(utimer, SIGNAL(timeout()), glwidget(), SLOT(update()));
    utimer->start();

    // Elapsed time timer
    timer = new QElapsedTimer();
    timer->start();

    // Define, compile & link shader
    QString src;
    src += "attribute vec3 P2;"; // coordenades del vèrtex al segon objecte (object space)
    src += "uniform float time;";
    src += "void main() {";
    src += "  vec3 interpolated = mix(gl_Vertex.xyz, P2, abs(cos(time)));";
    src += "  gl_FrontColor = vec4((gl_NormalMatrix * gl_Normal).z); ";
    src += "  gl_Position = gl_ModelViewProjectionMatrix * vec4(interpolated, 1.0); ";
    src += "}";
    vs = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vs->compileSourceCode(src);
    program = new QOpenGLShaderProgram(this);
    program->addShader(vs);
    program->link();
    qWarning() << vs->log() << endl;
    qWarning() << program->log() << endl;
    if (!program->isLinked())
    {
        qWarning() << ">>>> Hi ha errors al shader!" << endl;
	exit(1);
    }
}

bool Morphing::drawScene()
{	
    program->bind();
    program->setUniformValue("time", GLfloat(timer->elapsed()/1000.f));
    attributeLocation = program->attributeLocation("P2");
    //Scene* scene = scene();
    // Assumim que l'escena conté només dos objectes
    if (scene()->objects().size()!=2) {
	qWarning() << ">>>>> Per usar aquest plugin has de carregar exactament dos objectes!!!" << endl;
        exit(1);
    }
    const Object& obj = scene()->objects()[0];
    const Object& obj2 = scene()->objects()[1];
    if (obj.faces().size()!=obj2.faces().size())
    {
	qWarning() << ">>>>> Per usar aquest plugin els dos objectes han de tenir les mateixes cares!!!" << endl;
        exit(1);
    }
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
            const Point& p2 = obj2.vertices()[face.vertexIndex(v)].coord();
            program->setAttributeValue(attributeLocation, p2); // enviem les coordenades del vèrtex al segon objecte
            glVertex3f(p.x(), p.y(), p.z());
        }
        glEnd();
    }
    
    program->release();
    return true;
}

Morphing::~Morphing()
{
	delete timer;
	delete vs;
	delete program;
}

