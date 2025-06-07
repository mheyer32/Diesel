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
#include "image.h"
#include <memory.h>
#include <new>

#include <memorytracker.h>

Image::Image(int width, int height, int bytesperpixel, int destinationformat, const unsigned char* imagedata)
{
    init(width, height, bytesperpixel, destinationformat, -1, imagedata);
}

Image::Image(int width, int height, int bytesperpixel, int destinationformat, int srcformat,
             const unsigned char* imagedata)
{
    init(width, height, bytesperpixel, destinationformat, srcformat, imagedata);
}

Image::~Image(void)
{
    delete[] m_imagedata;
}

void Image::init(int width, int height, int bytesperpixel, int destinationformat, int srcformat,
                 const unsigned char* imagedata)
{
    m_imagedata = new unsigned char[width * height * bytesperpixel];

    m_width         = width;
    m_height        = height;
    m_bytesperpixel = bytesperpixel;
    m_destformat    = destinationformat;

    m_srcformat = srcformat == -1 ? m_destformat : srcformat;

    if (imagedata) {
        memcpy(m_imagedata, imagedata, getSize());
    }
}

unsigned char* Image::getImageData()
{
    return m_imagedata;
}
int Image::getWidth() const
{
    return m_width;
}
int Image::getHeight() const
{
    return m_height;
}
int Image::getBytesPerPixel() const
{
    return m_bytesperpixel;
}
int Image::getDestinationFormat() const
{
    return m_destformat;
}
int Image::getSourceFormat() const
{
    return m_srcformat;
}
int Image::getSize() const
{
    return m_width * m_height * m_bytesperpixel;
}
int Image::getNumPixels() const
{
    return m_width * m_height;
}

Image* Image::copy() const
{
    Image* copy = new Image(m_width, m_height, m_bytesperpixel, m_destformat, m_srcformat, m_imagedata);
    return copy;
}