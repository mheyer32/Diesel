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

#pragma once

#include <string>
#include "ImageLoader.h"

class TGALoader : public ImageLoader
{
public:
    TGALoader();
    virtual ~TGALoader();

    virtual Image* load(CFile& file);

protected:
#pragma pack(push, 1)
    struct _TGAHEADER
    {
        unsigned char  IDField;
        unsigned char  CMType;
        unsigned char  IMGType;
        unsigned short CMStart;
        unsigned short CMLength;
        unsigned char  CMBits;
        unsigned short X;
        unsigned short Y;
        unsigned short Width;
        unsigned short Height;
        unsigned char  IMGPixSize;
        unsigned char  IMGDescriptor;
    } header;
#pragma pack(pop)

    typedef void (TGALoader::*READBITMAP)(unsigned int PixelCount);

    void read_Uncompressed_RGB(unsigned int PixelCount);
    void read_RLE_RGB(unsigned int PixelCount);

    READBITMAP readBitmap;

    unsigned char* texmem;
    unsigned int   memPos;
    unsigned int   memSize;
    unsigned int   bytesperpixel;

    CFile* file;
};
