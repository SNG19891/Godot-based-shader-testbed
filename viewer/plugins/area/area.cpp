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

#include "area.h"
#include "glwidget.h"


void Area::onPluginLoad() {
    n_objectes = scene()->objects().size();
    for (unsigned i = 0; i < n_objectes; ++i) {
        const Object &obj = scene()->objects()[i];
        float area_tot = 0;
        for (unsigned j = 0; j < obj.faces().size(); ++j) {
            const Faces &f = obj.faces()[j];
            Point P0 = obj.vertices()[f.vertexIndex(0)].coord();
            Point P1 = obj.vertices()[f.vertexIndex(1)].coord();
            Point P2 = obj.vertices()[f.vertexIndex(2)].coord();
            Vector v1 = P0 - P1;
            Vector v2 = P0 - P2;
            area_tot += //fer mod prod vec de v1 v2
        }
        cout << "Objecte " << i << " Area: " << area_tot << endl;
    } 
}


