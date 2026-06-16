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

#ifndef _TEMPLATE_H
#define _TEMPLATE_H

#include "plugin.h"
#include <QGLShader>
#include <QGLShaderProgram>
#include <QElapsedTimer>

// Documentation: http://www.cs.upc.edu/~virtual/Gdocs/interfaces/html/class_basic_plugin.html

class Borra : public QObject, public Plugin
 {
     Q_OBJECT
     Q_PLUGIN_METADATA(IID "Plugin")   
     Q_INTERFACES(Plugin)

 public:

    void onPluginLoad();
    void onObjectAdd();
    void preFrame();
    void postFrame();
    bool paintGL();
    bool drawScene();
	
    void keyPressEvent (QKeyEvent *);
    void keyReleaseEvent (QKeyEvent *);
    void mouseMoveEvent (QMouseEvent *);
    void mousePressEvent (QMouseEvent *);
    void mouseReleaseEvent (QMouseEvent *);
    void wheelEvent (QWheelEvent *);

 public slots:

 private:
    QGLShaderProgram* program;
    QGLShader* vs;
    QGLShader* fs;

    QTimer timer;
    QElapsedTimer elapsedTimer;
    // Add here additional members
    

 };
 
 #endif

