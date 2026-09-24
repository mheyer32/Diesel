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

#include <math/MathLib.h>
#include "Transformation.h"

Transformation::Transformation()
{
    setIdentity();
    change_count = 0;
}

Transformation::~Transformation()
{
}

void Transformation::evaluateMatrix()
{
    // Reihenfolge der Rotationen : Z - X - Y  M*v= T*Ry*Rx*Rz*S*v
    const float cz = cosf(angles.z);
    const float sz = sinf(angles.z);
    const float cy = cosf(angles.y);
    const float sy = sinf(angles.y);
    const float cx = cosf(angles.x);
    const float sx = sinf(angles.x);

    matrix.m[0] = sy * sx * sz + cy * cz * scale;
    matrix.m[1] = cx * sz;
    matrix.m[2] = cy * sx * sz - sy * cz * scale;

    matrix.m[4] = -sz * cy + cz * sx * sy * scale;
    matrix.m[5] = cz * cx * scale;
    matrix.m[6] = sz * sy + cz * sx * cy * scale;

    matrix.m[8]  = cx * sy * scale;
    matrix.m[9]  = -sx * scale;
    matrix.m[10] = cx * cy * scale;
}

void Transformation::evaluateInverseMatrix()
{
    if (normalized_axes) {
        SimpleInverse4(matrix, inverse_matrix);
    } else {
        FullInverse4(matrix, inverse_matrix);
    }
    inverse_matrix_good = true;
}

Transformation& Transformation::operator=(const Transformation& trafo2)
{
    // matrix is always valid
    matrix          = trafo2.matrix;
    matrix_identity = trafo2.matrix_identity;
    normalized_axes = trafo2.normalized_axes;

    // only copy if valid
    inverse_matrix_good = trafo2.inverse_matrix_good;
    if (inverse_matrix_good)
        inverse_matrix = trafo2.inverse_matrix;

    angles_good = trafo2.angles_good;
    if (angles_good)
        angles = trafo2.angles;

    scale = trafo2.scale;  // FIXME: check if handling of scaling is supported correctly

    ++change_count;  // keep our own change count!
    return *this;
}
