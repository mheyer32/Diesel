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
#pragma once

/** this class is intended to be just a conatiner of image data */

class Image
{
public:
    /** create an image object of the given size and bitdepth, if imagedata points to image data
        in memory, this data will be copied into the newly created image object
        bytesperpixel is how many bytes will be occupied by one pixel
        destinationformat is an arbitrary constant that is used later when uploading the imagedata
        to the videocard. For now it will be used with GL textures as their intended internal format
    */
    Image(int width, int height, int bytesperpixel, int destinationformat, const unsigned char* imagedata = 0);

    // same as above, but must be used if format of image in memory differs from destinationformat
    Image(int width, int height, int bytesperpixel, int destinationformat, int srcformat,
          const unsigned char* imagedata = 0);

    virtual ~Image(void);

    unsigned char* getImageData();
    int            getWidth() const;
    int            getHeight() const;
    int            getBytesPerPixel() const;
    int            getDestinationFormat() const;
    int            getSourceFormat() const;
    int            getSize() const;  // return memorysize of whole image
    int            getNumPixels() const;

    Image* copy() const;

protected:
    void init(int width, int height, int bytesperpixel, int destinationformat, int srcformat,
              const unsigned char* imagedata);

    Image& operator=(const Image& image2);
    Image(const Image& image2);

    int m_width;   // width in pixels
    int m_height;  // height in pixels (no pitch assumed)

    int m_bytesperpixel;  // bytes per pixel
    int m_destformat;  // destination format in 3d api FIXME: introduce own format ids, match them later to real formats
    int m_srcformat;

    unsigned char* m_imagedata;  // data
};
