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
// JPGLoader.cpp: Implementierung der Klasse JPGLoader.
//
//////////////////////////////////////////////////////////////////////

#include "JPGLoader.h"

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

#include <gl/gl.h>
#include "gl/glext.h"

#include <iostream>

#include <MemoryTracker.h>
#include <jpeglib.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

#include <MemoryTracker.h>

JPGLoader::JPGLoader()
{
}

JPGLoader::~JPGLoader()
{
}

Image* JPGLoader::load(CFile& file)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr         jerr;
    ;

    int jpglen = file.getSize();

    unsigned char* filebuf = new unsigned char[jpglen];

    file.readVOID(filebuf, jpglen);

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, filebuf, jpglen);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    if (cinfo.output_components != 3) {
        std::cout << "JPGLoader::load(): Bad number of jpg components" << std::endl;
        return NULL;
    }

    // FIXME: usually JPEG files are SRGB encoded, but in this case I'd need to make the framebuffer sRGB as well
    Image* image = new Image(cinfo.output_width, cinfo.output_height, 3, /*GL_SRGB8_EXT*/ GL_RGB, GL_RGB, NULL);

    unsigned char* c = image->getImageData();

    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, &c, 1);
        c += cinfo.output_width * 3;
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    delete[] filebuf;

    return image;
}