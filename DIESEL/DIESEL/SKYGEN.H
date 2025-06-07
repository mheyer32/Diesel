/*
This file is part of Diesel
(c) 2002 by Mathias Heyer
email: sonode@gmx.de

Diesel is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Diesel is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef _SKYGEN_H
#define _SKYGEN_H

#include <math/vector3.h>
#include "enginetypes.h"

namespace SkyGen
{
enum
{
    SKY_MINS = 0,
    SKY_MAXS = 1,
    SKY_MINT = 2,
    SKY_MAXT = 3
};

enum
{
    SKY_RIGHT = 0,  // positive x
    SKY_TOP   = 1,  // positive y
    SKY_BACK  = 2,  // positive z

    SKY_LEFT   = 3,
    SKY_BOTTOM = 4,
    SKY_FRONT  = 5
};

extern void genSkyVerts(VECTOR3 forward, VECTOR3 up, VECTOR3 right, int tess, float skyboxsize, VECTOR3* vertices);
extern void genPlainSkyTexcoords(int tess, int textursize, VECTOR2* texcoords);
extern void genSphereTexcoords(float height, VECTOR3* vertices, int num_vertices, VECTOR2* texcoords);
extern int  genSkyIndices(const float* minmax, int tess, INDEX*& indices, int baseindex);
}  // namespace SkyGen

#endif