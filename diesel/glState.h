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

/* GL state caching */
#define GLSTATE_NORMALARRAY (1 << 3)
#define GLSTATE_COLORARRAY (1 << 4)
#define GLSTATE_BLENDING (1 << 5)
#define GLSTATE_ALPHATEST (1 << 6)
#define GLSTATE_DEPTHWRITE (1 << 7)
#define GLSTATE_LIGHTING (1 << 8)
#define GLSTATE_NOCULL (1 << 9)
#define GLSTATE_CULLFRONT (1 << 10)
#define GLSTATE_POLYGONOFFSET (1 << 11)
#define GLSTATE_DEPTHFUNC (1 << 12)
#define GLSTATE_FOG (1 << 13)
#define GLSTATE_NODEPTHTEST (1 << 14)

#define GLSTATE_MASK                                                                                                   \
    (GLSTATE_NORMALARRAY | GLSTATE_COLORARRAY | GLSTATE_BLENDING | GLSTATE_ALPHATEST | GLSTATE_DEPTHWRITE |            \
     GLSTATE_DEPTHFUNC | GLSTATE_LIGHTING | GLSTATE_NOCULL | GLSTATE_CULLFRONT | GLSTATE_POLYGONOFFSET |               \
     GLSTATE_DEPTHFUNC | GLSTATE_FOG | GLSTATE_NODEPTHTEST)

/* Texture Unit state caching */
#define TEXUNIT_ENABLED (1 << 0)  //
#define TEXUNIT_TCARRAY (1 << 1)  // TexCoordArray Enabled for this Unit
#define TEXUNIT_TCGEN (1 << 2)

#define TEXUNIT_MASK (TEXUNIT_ENABLED | TEXUNIT_TCARRAY | TEXUNIT_TCGEN)