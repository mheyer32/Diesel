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
#include "ROQTexture.h"

#include "engine.h"
#include <opengl/opengl.h>

#include <iostream>

#include <memorytracker.h>
using namespace std;

CROQTexture::CROQTexture(void)
{
	playing=false;
	fps=29;
	starttime=0;
	rinfo=NULL;
	framedata=NULL;
}

CROQTexture::~CROQTexture(void)
{
	close();

	if (framedata)
	{
		KILLARRAY(framedata);
	}
}
bool CROQTexture::open(const std::string &filename)
{
	cout<<"CROQTexture::openROQ(): "<<filename<<endl;
	rinfo = roq_open(const_cast<char*>(filename.c_str()));
	if(rinfo == NULL)
	{
		cout<<"CROQTexture::openROQ(): roq_open() failed, probably file not found"<<endl;
		return false;
	}

	cout<<"RoQ: length "<<rinfo->stream_length/60000<<":"<<((rinfo->stream_length + 500)/1000) % 60<<" video: "<<rinfo->width<<"x"<<rinfo->height<<" "<<rinfo->num_frames<<" frames  audio: "<<rinfo->audio_channels<<" channels"<<endl;

	width=rinfo->width;
	height=rinfo->height;
	
	if (!reserveTexture(width,height,GL_RGB))
	{
		cout<<"CROQTexture::openROQ()->CTexture::reserveTexture() failed"<<endl;
		return false;
	}

	framedata=new unsigned char[width*height*3];

	return true;	
}
void CROQTexture::close()
{
	if (rinfo)
	{
		roq_close(rinfo);
	}
}

void CROQTexture::grabFrame(int framenum)
{
//#define LIMIT(x) ((x)<0xFFFF)?(x)>>16:0xFF;
#define LIMIT(x) ((((x) > 0xffffff) ? 0xff0000 : (((x) <= 0xffff) ? 0 : (x) & 0xff0000)) >> 16)

	int ret = roq_read_frame(rinfo);
	if(ret <= 0)
	{
		//cout<<"CROQTexture::grabFrame()->roq_read_frame() returned <= 0"<<endl;
		return;
	}

	unsigned char *pa=rinfo->y[0];
	unsigned char *pb=rinfo->u[0];
	unsigned char *pc=rinfo->v[0];
	int pixel=0;
	int num_columns=width>>1;

	for(int y = 0; y < height; ++y)
	{
		for(int x = 0; x < num_columns; ++x)
		{
			int r, g, b, y1, y2, u, v, t;
			y1 = *(pa++); y2 = *(pa++);
			u = pb[x] - 128;
			v = pc[x] - 128;

			y1 <<= 16;
			y2 <<= 16;
			r = 91881 * v;
			g = -22554 * u + -46802 * v;
			b = 116130 * u;

			t=r+y1;
			framedata[pixel] =(unsigned char) LIMIT(t);
			t=g+y1;
			framedata[pixel+1] =(unsigned char) LIMIT(t);
			t=b+y1;
			framedata[pixel+2] =(unsigned char) LIMIT(t);
			t=r+y2;
			framedata[pixel+3] =(unsigned char) LIMIT(t);
			t=g+y2;
			framedata[pixel+4] =(unsigned char) LIMIT(t);
			t=b+y2;
			framedata[pixel+5] =(unsigned char) LIMIT(t);
			pixel+=6;
		}
		if(y & 0x01) { pb += num_columns; pc += num_columns; }
	}

	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, framedata);
	GLERROR("CROQTexture::grabFrame() glTexSubImage2D()");;
}

void CROQTexture::bindTexture()
{
	glBindTexture(GL_TEXTURE_2D, Texture);
	if (playing)
	{
		float frametime=g_Time-starttime;
		int newframe=(int)(fps*frametime);
		newframe=newframe%rinfo->num_frames;
		if (newframe!=frame)
		{
			frame=newframe;
			grabFrame(frame);
		}
	}
}

void CROQTexture::play()
{
	starttime=g_Time;
	playing=true;
}
void CROQTexture::stop()
{
	playing=false;
}

