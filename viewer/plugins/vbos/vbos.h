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

#ifndef _VBOS_H
#define _VBOS_H

#include <vector>
#include "plugin.h"

using namespace std;

class VBOs : public QObject, public Plugin
{
     Q_OBJECT
     Q_PLUGIN_METADATA(IID "Plugin")   
     Q_INTERFACES(Plugin)

 public:
     ~VBOs();
 
     bool drawScene();
     void onPluginLoad();
     void onObjectAdd();
    
 private:
    void addVBO(unsigned int objectID);

    // one VBO for each object in the scene
    vector<GLuint> idsV;    // ID of array of vertex coordinates (x,y,z,x,y,z...)
    vector<GLuint> idsN;    // ID of array of normal components (nx,ny,nz,nx,ny,nx)
    vector<GLuint> idsF;    // ID of array of indices (i0, i1, i2, ...)
    vector<GLuint> numIndices;  // Num indices in each array of indices  
};
 
#endif

