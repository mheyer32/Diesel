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
#include "WaterTexture.h"

#include <math.h>
#include <iostream>

#include <gl/glu.h>
#include <opengl/OpenGL.h>
#include "Engine.h"

#include <MemoryTracker.h>
using namespace std;

ConVar CWaterTexture::r_showWaterWaves("r_showWaterWaves", "0");

CWaterTexture::CWaterTexture(void)
{
    water1    = NULL;
    water2    = NULL;
    destimage = NULL;
    width = height = 0;
    lastupdate     = 0;

    fps = 15;

    addr = 0;

    amp   = 32767;
    freq  = PI / 10;
    phase = 0;
    damp  = 0.99f;
}

CWaterTexture::~CWaterTexture(void)
{
    KILLARRAY(water1);
    KILLARRAY(water2);
    KILLOBJECT(image)
    KILLOBJECT(destimage);
}

void CWaterTexture::setFPS(float newFPS)
{
    fps = newFPS;
}

bool CWaterTexture::setImage(Image* newimage, int Options)
{
    loadoptions = Options;

    width  = newimage->getWidth();
    height = newimage->getHeight();

    chooseSize(width, height);

    maskx          = width - 1;
    masky          = height - 1;
    buffermask     = width * height - 1;
    shiftx         = 0;
    unsigned int t = width;
    while (t != 1) {
        shiftx++;
        t >>= 1;
    }

    int format = newimage->getDestinationFormat();
    format     = chooseInternalFormat(format, 0);  // don´t compress

    if (!reserveTexture(width, height, format)) {
        std::cout << "CWaterTexture::setImage() could not reserve texture" << endl;
        return false;
    }

    int memsize = width * height;

    water1 = new short[memsize];
    water2 = new short[memsize];

    ZeroMemory(water1, memsize * sizeof(short));
    ZeroMemory(water2, memsize * sizeof(short));

    image     = new Image(width, height, 4, format);
    destimage = image->copy();

    setTexParameter(Options | TEX_NOMIPMAP, TEX_CLAMP | TEX_NOMIPMAP);
    setTextureMode(texturemode);

    gluScaleImage(newimage->getBytesPerPixel(),
                  newimage->getWidth(),
                  newimage->getHeight(),
                  GL_UNSIGNED_BYTE,
                  newimage->getImageData(),
                  width,
                  height,
                  GL_UNSIGNED_BYTE,
                  image->getImageData());
    GLERROR("CWaterTexture::setImage() -> gluScaleImage() failed:");

    // convert(Bitmap);

    bindTexture();

    return true;
}

/*void CWaterTexture::convert(const ARGBBITMAP &Bitmap)
{
    if (Bitmap.format==GL_RGBA||Bitmap.format==4||Bitmap.format==GL_RGBA8)
    {
        return;
    }
    if (Bitmap.format==GL_RGB||Bitmap.format==3||Bitmap.format==GL_RGB8)
    {
        unsigned char *src=(unsigned char *)teximage;
        int j=0;
        for (unsigned int i=0;i<width*height;++i)
        {
            destimage[i]=(src[j])|(src[j+1]<<8)|(src[j+2]<<16);
            j+=3;
        }
        unsigned int *tempptr=teximage;
        teximage=destimage;
        destimage=tempptr;
    }
}
*/
/** bind the texture and (if set) proper material settings*/
void CWaterTexture::bindTexture()
{
    // std::cout<<"CWaterTexture::bindTexture()"<<std::endl;
    CTexture::bindTexture();

    float time_passed = g_Time - lastupdate;

    if (time_passed * fps >= 1.0f) {
        lastupdate = g_Time;
        Water(water1,
              water2,
              width,
              height,
              (const unsigned int*)image->getImageData(),
              (unsigned int*)destimage->getImageData());

        short* temp = water1;
        water1      = water2;
        water2      = temp;

        if ((unsigned int)rand() < 200) {
            addr = rand() % (width * height);
            amp  = 32767;
        }

        water1[addr] = sinf(phase) * amp;
        phase += freq;
        amp *= damp;

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, destimage);
        // GLERROR("void CWaterTexture::bindTexture() -> glTexSubImage2D () failed");
    }
}

void CWaterTexture::Water(short*&             current_buffer,
                          short*&             old_buffer,
                          int                 width,
                          int                 height,
                          const unsigned int* sourcetex,
                          unsigned int*       desttex)
{

    for (int y = 1; y < height - 1; ++y) {
        int ax = (y << shiftx) + 1;
        for (int x = 1; x < width - 1; ++x, ++ax) {
            int temp = (((  // current_buffer[ax-width-1]
                             +current_buffer[ax - width]
                             //+current_buffer[ax-width+1]
                             //+current_buffer[ax+width-1]
                             + current_buffer[ax + width]
                             //+current_buffer[ax+width+1]
                             + current_buffer[ax - 1] + current_buffer[ax + 1]) >>
                         1) -
                        old_buffer[ax]);

            // damping
            // temp-=temp/150;
            temp -= temp >> 6;
            // clamping
            // if (temp>32767) temp=32767;
            // if (temp<-32767) temp=-32767;

            old_buffer[ax] = (short)temp;
        }
    }

    if ((int)r_showWaterWaves) {
        for (int p = 0; p < width * height; ++p) {
            short value = old_buffer[p];
            value >>= 8;
            if (value < -127)
                value = -127;
            if (value > 127)
                value = 127;
            value += 127;
            desttex[p] = value | (value << 8) | (value << 16);  //|(value<<24);
        }
        return;
    }

    // refractionmapping
    for (int y = 0; y < height; ++y) {
        int addr = y << shiftx;
        for (int x = 0; x < width; ++x, ++addr) {
            int dx = old_buffer[addr] - old_buffer[(addr + 1) & maskx];
            dx >>= 5;  // reduce strength of refraction
            dx += x;
            dx &= maskx;

            int dy = old_buffer[addr] - old_buffer[(addr + width) & buffermask];
            dy >>= 5;
            dy += y;
            dy &= masky;

            desttex[addr] = sourcetex[dx + (dy << shiftx)];

            // int px=(x+dx)&maskx;
            // if (px<0) px=0;
            // else if (px>width-1) px=width-1;
            //			int py=(y+dy)&masky;
            //			if (py<0) py=0;
            //			else if (py>height-1) py=height-1;
            //			px+=py*width;
            //			desttex[addr] =sourcetex[px];
        }
    }
}