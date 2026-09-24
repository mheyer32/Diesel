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
// MESHLoader.h: Schnittstelle für die Klasse MESHLoader.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHLOADER_H__94E5697A_DFCF_45A5_AE42_FF46FED9021F__INCLUDED_)
#define AFX_MESHLOADER_H__94E5697A_DFCF_45A5_AE42_FF46FED9021F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <file/File.h>
#include <math/Vector2.h>
#include <math/Vector3.h>

#include "Color.h"  // Hinzugefügt von der Klassenansicht

#include <vector>

class CMesh;
class CShader;
class CVertexBuffer;

class MESHLoader
{
public:
    MESHLoader();
    virtual ~MESHLoader();

    CMesh* loadMESH(std::string filename);

protected:
    COLOR   readColor();
    VECTOR3 readVector3();
    VECTOR2 readVector2();

    CVertexBuffer* readVertexBuffer();

    std::vector<CShader*> shaders;
    CFile*                file;
};

#endif  // !defined(AFX_MESHLOADER_H__94E5697A_DFCF_45A5_AE42_FF46FED9021F__INCLUDED_)
