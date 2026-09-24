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
#ifndef FEATURES_H
#define FEATURES_H

#include <math/Matrix4.h>
#include <math/Plane.h>
#include <math/Vector3.h>
#include "Shader.h"

enum FEATURENAME
{
    FEATURE_FOG,       // data is pointing to a FOGFEATURE structure
    FEATURE_PATCHDIM,  // data points to int patchdim[2],
    FEATURE_DLIGHT
};

struct FEATURE
{
    FEATURENAME name;
    void*       data;  // note that the address given here must be on the heap!
};

struct FOGFEATURE
{
    PLANE             plane;     // the plane where the fog starts
    CShader::SMARTPTR shader;    // the shader (contains distance, fogcolor)
    int               brushnum;  // not used by renderer
};

struct LIGHT;

struct DLIGHTFEATURE
{
    MATRIX4 tangentspace;
    LIGHT*  light;
    int     use_reallight;
};

#endif