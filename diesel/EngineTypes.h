/*
This file is part of Diesel
(c) 2002-2026 by Mathias Heyer
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
#if !defined ENGINE_TYPES_INCLUDED
#define ENGINE_TYPES_INCLUDED

#include <geometry\AABB.h>
#include <math\Matrix3.h>
#include <math\Matrix4.h>
#include <math\Plane.h>
#include <math\Vector2.h>
#include <math\Vector3.h>
#include <math\Vector4.h>
#include "Color.h"
#include "Features.h"
//////////////////////////////////////////////////////////////////////
/*////////////////////////////////////////////////////////////////////
Defines
////////////////////////////////////////////////////////////////////*/

/*////////////////////////////////////////////////////////////////////
Types
////////////////////////////////////////////////////////////////////*/

//////////////////////////////////////////////////////////////////////////////////////////////

typedef unsigned short INDEX;
#define GL_INDEX_ENUM GL_UNSIGNED_SHORT
#define MAX_INDEX 0xFFFF

class CVertexBuffer;
class CShader;

#define MAX_FEATURES 4

struct RENDERINFO
{
    unsigned int   sortkey;
    CVertexBuffer* vbuffer;
    CShader*       shader;
    int            num_features;
    FEATURE        features[MAX_FEATURES];

    RENDERINFO() { ZeroMemory(this, sizeof(RENDERINFO)); }
};

/*////////////////////////////////////////////////////////////////////
Function Prototypes
////////////////////////////////////////////////////////////////////*/

/*////////////////////////////////////////////////////////////////////////////////////////////////////////
        Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////*/
#endif
