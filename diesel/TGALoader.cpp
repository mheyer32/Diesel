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

#include "TGALoader.h"

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

#include <gl/gl.h>

#include <cassert>
#include <iostream>

#include <MemoryTracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

using namespace std;

TGALoader::TGALoader()
{
    texmem  = NULL;
    memPos  = 0;
    memSize = 0;
    file    = NULL;
}

TGALoader::~TGALoader()
{
}

Image* TGALoader::load(CFile& tgafile)
{
    file = &tgafile;

    file->readVOID(&header, sizeof(header));

    int bpp = header.IMGPixSize;

    switch (header.IMGType) {
    case 2:
    case 3:
        readBitmap = &TGALoader::read_Uncompressed_RGB;
        break;
    case 10:
    case 11:
        readBitmap = &TGALoader::read_RLE_RGB;
        break;
    default:
        std::cout << "TGALoader::LoadTGA() unsupported imagetype" << std::endl;
        return NULL;
    }

    int srcformat;
    int destformat;

    switch (bpp) {
    case 8:
        srcformat  = GL_LUMINANCE;  // Q3 specific!
        destformat = GL_LUMINANCE;
        break;
    case 24:
        destformat = GL_RGB;
        srcformat  = GL_RGB;
        break;
    case 32:
        destformat = GL_RGBA;
        srcformat  = GL_RGBA;
        break;
    default:
        std::cout << "TGALoader::LoadTGA() unsupported bits per pixel" << std::endl;
        return NULL;
    }

    bytesperpixel = bpp >> 3;

    Image* image = new Image(header.Width, header.Height, bytesperpixel, destformat, srcformat, NULL);

    texmem = image->getImageData();

    file->setPosition(file->getPosition() + header.IDField);  // skip ID field

    (this->*readBitmap)(image->getNumPixels());

    if (bpp == 24) {
        int size = image->getSize();
        for (int p = 0; p < size; p += 3) {
            unsigned char c = texmem[p];
            texmem[p]       = texmem[p + 2];
            texmem[p + 2]   = c;
        }
    } else if (bpp == 32) {
        int size = image->getSize();
        for (int p = 0; p < size; p += 4) {
            unsigned char c = texmem[p];
            texmem[p]       = texmem[p + 2];
            texmem[p + 2]   = c;
        }
    }

    return image;
}

void TGALoader::read_Uncompressed_RGB(unsigned int PixelCount)
{
    file->readVOID(texmem, PixelCount * bytesperpixel);
}

void TGALoader::read_RLE_RGB(unsigned int PixelCount)
{
    unsigned char  p[4];
    unsigned char* temptexmem = texmem;
    BYTE           h;

    while (PixelCount > 0) {
        h = file->readBYTE();
        memPos++;
        if (h & 0x80) {  //
            h &= 0x7f;
            h++;
            PixelCount -= h;  //

            file->readVOID(p, bytesperpixel);

            assert(PixelCount >= 0);

            for (; h != 0; --h) {
                for (int c = 0; c < bytesperpixel; ++c) {
                    *temptexmem++ = p[c];
                }
            }
            continue;
        } else {
            h++;
            PixelCount -= h;
            assert(PixelCount >= 0);

            file->readVOID(temptexmem, h * bytesperpixel);
            temptexmem += h * bytesperpixel;
        }
    }
}
