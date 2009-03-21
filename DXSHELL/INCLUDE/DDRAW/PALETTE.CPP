
/*
This file is part of DXShell
(c) 2002 by Mathias Heyer

DXShell is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

DXShell is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
// Palette.cpp: Implementierung der Klasse CPalette.
//
//////////////////////////////////////////////////////////////////////

#include "Palette.h"

#include <ddraw/ddutil.h>
#include "directdraw.h"

#include "ddrawexception.h"
#include <math.h> 
#include <defs.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////


CPalette::CPalette()
{
	for (int i=0; i<256; i++)
	{
		pe[i].peFlags=0;
	}
	DDPalette=NULL;

	Initialize();
}

CPalette::CPalette(LPCSTR szPalette)
{
	if (!setFromFile(szPalette)) throw CException("CPalette::CPalette()->setFromFile() failed");
}

CPalette::CPalette(LPDIRECTDRAWPALETTE Palette)
{	
	if (!setFrom(Palette))
	{
		throw CException("CPalette::CPalette()->createFrom() failed:\n ");
	}
}

CPalette::~CPalette()
{
	if (DDPalette!=NULL)
	{
		DDPalette->Release();
		DDPalette=NULL;
	}
	KILLARRAY(ShadeTable);
	KILLARRAY(FogTable);
	KILLARRAY(ConvertTableRGB);
	KILLARRAY(ConvertTableYUV);
}

inline void CPalette::Initialize()
{
	ShadeTable=NULL;
	FogTable=NULL;
	ConvertTableRGB=NULL;
	ConvertTableYUV=NULL;
	if (DDPalette!=NULL)
	{
		DDPalette->GetEntries(0,0,256,pe);
	}
}

bool CPalette::setFromFile(LPCSTR PalFilename)
{
LPDIRECTDRAWPALETTE tempPal;


	CDirectDraw::Instance()->getLPDD()->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256,pe,&DDPalette,NULL);

	tempPal = DDLoadPalette(CDirectDraw::Instance()->getLPDD(), PalFilename);

	ZeroMemory(pe,sizeof(pe));
	tempPal->GetEntries(0,0,256,pe);
	tempPal->Release();

	for (int i=0; i<256; i++){
		pe[i].peFlags|=PC_RESERVED;
	}
	HRESULT ddrval=DDPalette->SetEntries(0,0,256,pe);
	if (ddrval!=DD_OK)
	{
		throw CDDrawException("CPalette::setFromFile() SetPalette fehlgeschlagen",ddrval);
		return false;
	}
	
	Initialize();
	return true;
}

bool CPalette::setFrom(LPDIRECTDRAWPALETTE Palette)
{

	for (int i=0; i<256; i++)
	{
		pe[i].peFlags=0;
	}
	DDPalette=Palette;
	DDPalette->AddRef();	// now WE have a pointer to it, too

	Initialize();

	return true;
}

bool CPalette::setFromSurface(LPDDSURFACE Surface)
{
	HRESULT ddrval=Surface->GetPalette(&DDPalette);
	if (ddrval!=DD_OK) return false;

	Initialize();
	return true;
}
 /*Its also a good idea to weight the rgb distances differently. Your eyes are
 generally more sensitive to the green band of colour, then red, then blue.
 So weight them perhaps:

 0.30*Red
 0.59*Green
 0.11*Blue

 This usually improves quality a bit. Also, you can avoid the sqrt()
 function call, by just comparing the squared distance. I'm not sure if this
 works perfectly as a replacement, but it seems fine to me.
 */
int CPalette::getBestColor(float r,float g, float b)
{
	#define RW 0.30f
	#define GW 0.59f
	#define BW 0.11f
	 int             n;
	 int             bestcol = 0;
	 float          bestdist = 999999;
	 float          dist, rdist, gdist, bdist;
	 float          red, green, blue;

	 for(n=0; n<256; n++) {
             red   = (float) pe[n].peRed;
             green = (float) pe[n].peGreen;
             blue  = (float) pe[n].peBlue;
             rdist = (red   - r)*RW;
             gdist = (green - g)*GW;
             bdist = (blue  - b)*BW;
             dist = rdist * rdist + gdist * gdist + bdist * bdist;
			 //if(dist == 0) return n;
             if(dist < bestdist)
			 {
				 bestdist = dist;
				 bestcol = n;
             }
      }
	  return bestcol;
 }


/*
 Calculating The Colour For A Given Shade

 Calculating what colour to search for is simple enough. You can use any
 formulae you like for this, a phong lighting model, a transparency, a depth
 cue formula. The point is you need to calculate RGB for a given colour, and
 a given 'shade'.

 Phong Lighting Model

 The formula for this is:

 Ambient + Diffuse + Specular

 Which becomes:
 Ia*ka*Oa + I*(Od*kd*(N.L) + Os*ks*(R.V)^n)

 Ia is intensity of ambient
 Ka is ambient co-ef
 Oa is ambient colour. These are constant, set to whatever you like
 I is intensity of light. Say 1.0
 Od is colour for diffuse, replace with colour[n]
 Kd is diffuse co-efficient. About 0.95 - 1.0 is good
 (N.L) is angle of incidence, replace shade[n]
 Os is specular colour, say 255
 Ks is specular co-ef, say 0.75
 (R.V) is angle of reflection. Replace with spec[n]
 N is the shinyness. A value of around 20 looks fine.

 You'll need to calculate this for R G and B seperately. Make the N.L and
 (R.V)^n terms into lookup tables. Then search for the value in the palette,
 and you're done.

*/
/*
BYTE CPalette::getShadeOfColor(float red, float green, float blue, float shade, float specular)
{
#define	Ia 1
#define Ka 1
#define rOa 0
#define gOa 0
#define bOa 0
#define I 2.1
#define Kd (1/256.0)
#define rOs 0
#define gOs 0
#define bOs 50
#define Ks 1
#define n 20
float r,g,b,temp;

	 temp=Ks*pow(specular,n);
	 r=Ia*Ka*rOa + I*(red*Kd*shade + rOs*temp);
	 g=Ia*Ka*gOa + I*(green*Kd*shade + gOs*temp);
	 b=Ia*Ka*bOa + I*(blue*Kd*shade + bOs*temp);
	return BestColor(r,g,b);
}
*/

void CPalette::createShadeTable(int Threshold)
{
	if (ShadeTable==NULL)
	{
		ShadeTable=new BYTE [256*256];
	}

	//DDPalette->GetEntries(0,0,256,pe);

	float red,green,blue,redstep,greenstep,bluestep;
	for (int color=0;color<256;color++)
	{
		redstep = (float)pe[color].peRed/(float)Threshold;
		greenstep = (float)pe[color].peGreen/(float)Threshold;
		bluestep = (float)pe[color].peBlue/(float)Threshold;
		red=green=blue=0;
		for (int shade=0;shade<Threshold;shade++)
		{			
			red+=redstep;
			green+=greenstep;
			blue+=bluestep;
			ShadeTable[(color<<8)+shade]=getBestColor(red,green,blue);
		}
		redstep = (255.0f-(float)pe[color].peRed)/(255.0f-(float)Threshold);
		greenstep = (255.0f-(float)pe[color].peGreen)/(255.0f-(float)Threshold);
		bluestep = (255.0f-(float)pe[color].peBlue)/(255.0f-(float)Threshold);

		for (int shade=Threshold;shade<256;shade++)
		{
			red+=redstep;
			green+=greenstep;
			blue+=bluestep;
			ShadeTable[(color<<8)+shade]=getBestColor(red,green,blue);
		}
	}

}
/*
void CPalette::CreateFogTable(float rF,float gF,float bF, float density, float distance){
int color,dist;
float r,g,b,fr,fg,fb,f,d,dstep;
char c;
#define EXP2FOG

	if (FogTable==NULL)
		FogTable=new unsigned char [256*256];
	
	dstep=distance/256;
	d=0;

	for (color=0;color<256;color++){
			r=pe[color].peRed;
			g=pe[color].peGreen;
			b=pe[color].peBlue;
			f=1/256;
		for (dist=0;dist<256;dist++,d+=dstep){

#ifndef EXP2FOG
			f=1/(exp(d*density));
#else
	
			f=1/(exp(d*d*density*density));
#endif
			fr=f*r+(1-f)*rF;
			fg=f*g+(1-f)*gF;
			fb=f*b+(1-f)*bF;
			c=BestColor(fr,fg,fb);
			FogTable[(dist<<8)+color]=c;
			//Primary->SetPixel(color,shade,c);
		}
	}
}
*/
void CPalette::CreateConvertTableRGB(LPDDSURFACE DestSurface)
{
DWORD	rshift,gshift,bshift,r,g,b,rmask,gmask,bmask,
		rshift2,gshift2,bshift2;

DDPIXELFORMAT ddpf;

	
	if (ConvertTableRGB==NULL) ConvertTableRGB=new DWORD[256];

	DestSurface->GetPixelFormat(&ddpf);

	rshift=gshift=bshift=0;
	rshift2=gshift2=bshift2=8;
	rmask=ddpf.dwRBitMask;
	gmask=ddpf.dwGBitMask;
	bmask=ddpf.dwBBitMask;
	while (!(rmask&1)) { // Nullen zählen
		rmask>>=1;
		rshift++;
	}

	while (!(gmask&1)) {
		gmask>>=1;
		gshift++;
	}
	
	while (!(bmask&1)) {
		bmask>>=1;
		bshift++;
	}
	while (rmask){	// Einsen zählen
		rmask>>=1;
		rshift2--;
	}	
	while (gmask){
		gmask>>=1;
		gshift2--;
	}
	while (bmask){
		bmask>>=1;
		bshift2--;
	}
	
	rshift-=rshift2;
	gshift-=gshift2;
	bshift-=bshift2;

	for (int color=0; color<256; ++color)
	{
		r=((DWORD)pe[color].peRed<<rshift)&ddpf.dwRBitMask;
		g=((DWORD)pe[color].peGreen<<gshift)&ddpf.dwGBitMask;
		b=((DWORD)pe[color].peBlue<<bshift)&ddpf.dwBBitMask;
		ConvertTableRGB[color]=(r|g|b);
	};

}

void CPalette::CreateConvertTableYUV(LPDDSURFACE DestSurface)
{

}