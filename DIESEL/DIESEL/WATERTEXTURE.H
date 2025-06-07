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
#include "texture.h"

#include <adt/ConVar.h>

class CWaterTexture : public CTexture
{
public:
    CWaterTexture();
    virtual ~CWaterTexture(void);

    /** sets the given Bitmap as new Image
    note that this function will
    completely destroy the old texture and create a new one
    TEX_KEEPIMAGE will be used by default
    */
    virtual bool setImage(Image* newimage, int Options);

    /** bind the texture and (if set) proper material settings*/
    virtual void bindTexture();

    void setFPS(float newFPS);

protected:
    void Water(short*& current_buffer, short*& old_buffer, int width, int height, const unsigned int* sourcetex,
               unsigned int* desttex);
    //	void convert(const ARGBBITMAP &Bitmap);

    short* water1;
    short* water2;

    unsigned int maskx;
    unsigned int shiftx;
    unsigned int masky;
    unsigned int buffermask;

    Image* destimage;

    float fps;
    float lastupdate;

    float amp;
    float freq;
    float phase;
    float damp;

    unsigned int addr;

    static ConVar r_showWaterWaves;
};
