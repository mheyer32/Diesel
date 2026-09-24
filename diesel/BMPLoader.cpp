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
// BMPLoader.cpp: Implementierung der Klasse BMPLoader.
//
//////////////////////////////////////////////////////////////////////

#include <opengl/OpenGL.h>  // for GL_BGRA_EXT
#include "BMPLoader.h"

#include <iostream>

#include <MemoryTracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

#define BI_RGB 0L
#define BI_RLE8 1L
#define BI_RLE4 2L
#define BI_BITFIELDS 3L

BMPLoader::BMPLoader()
{
}

BMPLoader::~BMPLoader()
{
}

Image* BMPLoader::load(CFile& file)
{
    BITMAPFILEHEADER bmfh;

    file.readVOID(&bmfh, sizeof(bmfh));

    if (bmfh.bfType != (('M' << 8) | 'B')) {
        std::cout << "BMPLoader::LoadBMP() file is not a BMP" << std::endl;
    }

    BITMAPINFOHEADER bmih = {0};
    BITMAPCOREHEADER bmic = {0};

    int startheader = file.getPosition();

    int headersize = file.readDWORD();

    int width;
    int height;
    int bpp;  // bitsperpixel
    if (headersize == sizeof(bmih)) {
        file.setPosition(startheader);
        file.readVOID(&bmih, sizeof(bmih));
        width  = bmih.biWidth;
        height = bmih.biHeight;
        bpp    = bmih.biBitCount;
    } else if (headersize == sizeof(bmic)) {
        file.setPosition(startheader);
        file.readVOID(&bmic, sizeof(bmic));
        width  = bmic.bcWidth;
        height = bmic.bcHeight;
        bpp    = bmic.bcBitCount;
    } else {
        std::cout << "BMPLoader::LoadBMP() unknown header format" << std::endl;
    }

    int memformat;
    int dstformat;
    int bytesperpixel;

    switch (bpp) {
    case 24:
        memformat     = GL_BGR_EXT;
        dstformat     = GL_RGB;
        bytesperpixel = 3;
        break;
    case 32:
        memformat     = GL_BGRA_EXT;
        dstformat     = GL_RGBA;
        bytesperpixel = 4;
        break;
    default:
        std::cout << "BMPLoader::LoadBMP() bmp bitcount other than 24 or 32 not supported" << std::endl;
        return NULL;
        break;
    }

    if (bmih.biCompression != BI_RGB) {
        std::cout << "BMPLoader::LoadBMP() compressed image data not supported" << std::endl;
        return false;
    }

    Image* image = new Image(width, height, bytesperpixel, dstformat, memformat);

    file.setPosition(startheader + headersize);
    file.readVOID(image->getImageData(), image->getSize());

    return image;
}
