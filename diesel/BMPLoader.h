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
// BMPLoader.h: Schnittstelle für die Klasse BMPLoader.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMPLOADER_H__4CB7B5C4_3CBE_46E6_AA48_85D25FB183C1__INCLUDED_)
#define AFX_BMPLOADER_H__4CB7B5C4_3CBE_46E6_AA48_85D25FB183C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "ImageLoader.h"

class BMPLoader : public ImageLoader
{
public:
    BMPLoader();
    virtual ~BMPLoader();

    virtual Image* load(CFile& file);

protected:
#pragma pack(push, 2)
    struct BITMAPFILEHEADER
    { /* bmfh */
        unsigned short bfType;
        unsigned int   bfSize;
        unsigned short bfReserved1;
        unsigned short bfReserved2;
        unsigned int   bfOffBits;
    };

    struct BITMAPCOREHEADER
    {
        unsigned int   bcSize;
        unsigned short bcWidth;
        unsigned short bcHeight;
        unsigned short bcPlanes;
        unsigned short bcBitCount;
    };

    struct BITMAPINFOHEADER
    { /* bmih */
        unsigned int   biSize;
        unsigned long  biWidth;
        unsigned long  biHeight;
        unsigned short biPlanes;
        unsigned short biBitCount;
        unsigned int   biCompression;
        unsigned int   biSizeImage;
        unsigned long  biXPelsPerMeter;
        unsigned long  biYPelsPerMeter;
        unsigned int   biClrUsed;
        unsigned int   biClrImportant;
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct RGBQUAD
    { /* rgbq */
        unsigned char rgbBlue;
        unsigned char rgbGreen;
        unsigned char rgbRed;
        unsigned char rgbReserved;
    };
#pragma pack(pop)
};

#endif  // !defined(AFX_BMPLOADER_H__4CB7B5C4_3CBE_46E6_AA48_85D25FB183C1__INCLUDED_)
