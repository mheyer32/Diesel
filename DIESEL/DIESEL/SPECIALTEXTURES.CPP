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
#include "specialtextures.h"
#include <math.h>

#include <memorytracker.h>

CTexture* SpecialTextures::createDLightTexture()
{
#define DLIGHT_WIDTH 64

	Image *dlightimage=new Image (DLIGHT_WIDTH,DLIGHT_WIDTH,1,GL_LUMINANCE);

	unsigned char *data=dlightimage->getImageData();
	float fy=-0.5f;
	float df;
	df=1.0f/DLIGHT_WIDTH;
	float val;

	for (int y=0; y<DLIGHT_WIDTH; ++y)
	{
		float fx=-0.5f;
		for (int x=0; x<DLIGHT_WIDTH; ++x)
		{
			val=sqrtf(fx*fx+fy*fy);
			if (val>0.5f) val=0.5f;
			data[y*DLIGHT_WIDTH+x]=255-(unsigned char)(val*511.0f);
			fx+=df;
		}
		fy+=df;
	}

	CTexture *dlighttexture=new CTexture;
	dlighttexture->setName("DLIGHT_texture");
	dlighttexture->setImage(dlightimage,TEX_NOPICMIP|TEX_NOMIPMAP|TEX_CLAMP|TEX_KEEPIMAGE|TEX_DONTCOMPRESS);

	return dlighttexture;
}


CTexture* SpecialTextures::createWhitemap()
{
	Image *whiteimage=new Image(1,1,3,GL_RGB);

	unsigned char* whitedata=whiteimage->getImageData();
	whitedata[0]=whitedata[1]=whitedata[2]=255;

	CTexture *whitemap=new CTexture;
	whitemap->setName("$whiteimage");
	whitemap->setImage(whiteimage,TEX_NOPICMIP|TEX_NOMIPMAP|TEX_CLAMP|TEX_KEEPIMAGE|TEX_DONTCOMPRESS);

	return whitemap;
}


#define FOG_WIDTH 256
#define FOG_HEIGHT 32

CTexture* SpecialTextures::createFogTexture()
{

	Image *fogimage=new Image(FOG_WIDTH,FOG_HEIGHT,1,GL_ALPHA);

	unsigned char *fogdata=fogimage->getImageData();

	float tw=1.0f/((float)fogimage->getWidth()-1.0f);
	float th=1.0f/((float)fogimage->getHeight()-1.0f);
	float tx=0.0f;
	float ty=0.0f;
	float a;
	for (int y=1;y<fogimage->getHeight();++y)
	{
		tx=0.0f;
		for (int x=0;x<fogimage->getWidth();++x)
		{
			//a=tx*255.0f;  // LINEAR
			// squared fog-thickness proportional to linear distance seems to be best looking
			a=sqrtf(tx)*255.0f;  //SQUARE
			if (a>255.0f) a=255.0f;
			fogdata[(y*fogimage->getWidth())+x]=(unsigned char)a;
			tx+=tw;
		}
		ty+=th;
	}
	for (int x=0;x<fogimage->getWidth();++x)
	{
		fogdata[x]=0;
	}

	CTexture *fogtexture=new CTexture;

	fogtexture->setName("fogtexture");
	fogtexture->setImage(fogimage,TEX_NOMIPMAP|TEX_NOPICMIP|TEX_CLAMP|TEX_KEEPIMAGE|TEX_DONTCOMPRESS);

	return fogtexture;
}