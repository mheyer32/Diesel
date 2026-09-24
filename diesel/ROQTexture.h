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

#include <file/Path.h>
#include "VideoTexture.h"

#ifdef _MSC_VER
extern "C" { /* Assume C declarations for C++ */
#include "roqplayer/roq.h"
}
#else
#include "roqplayer/roq.h"
#endif

class CROQTexture : public CVideoTexture
{
public:
    CROQTexture(void);
    virtual ~CROQTexture(void);

    virtual void bindTexture();

    virtual void play();
    virtual void stop();  // fixme rewind, fforward etc missing

    virtual bool open(const std::string& filename);
    virtual void close();

protected:
    void grabFrame(int framenum);

    roq_info* rinfo;

    float starttime;
    bool  playing;
    float fps;
    int   frame;

    unsigned char* framedata;
};
