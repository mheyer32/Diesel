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
#ifndef _LIGHT_H
#define _LIGHT_H

#include <math/vector3.h>
#include <math/vector4.h>

#define LIGHT_DIFFUSE  (1<<0)
#define LIGHT_AMBIENT  (1<<1)
#define LIGHT_SPECULAR (1<<2)
#define LIGHT_POSITION (1<<3)
#define LIGHT_DIRECTION (1<<4)
#define LIGHT_CUTOFF	(1<<5)
#define LIGHT_LINEAR_ATTENUATION (1<<6)
#define LIGHT_CONSTANT_ATTENUATION (1<<7)
#define LIGHT_QUADRATIC_ATTENUATION (1<<8)
#define LIGHT_EXPONENT (1<<9)

#define DIRECTIONAL_LIGHT (LIGHT_DIFFUSE|LIGHT_AMBIENT|LIGHT_SPECULAR|LIGHT_POSITION|LIGHT_CUTOFF)
#define SPOT_LIGHT		  (LIGHT_DIFFUSE|LIGHT_AMBIENT|LIGHT_SPECULAR|LIGHT_POSITION|LIGHT_DIRECTION|LIGHT_CUTOFF|LIGHT_ATTENUATION|LIGHT_EXPONENT)
#define POINT_LIGHT		  (LIGHT_DIFFUSE|LIGHT_AMBIENT|LIGHT_SPECULAR|LIGHT_POSITION|LIGHT_CUTOFF|LIGHT_QUADRATIC_ATTENUATION|LIGHT_EXPONENT)

struct LIGHT
{
	VECTOR4 pos;
	VECTOR3 dir;
	VECTOR4 diffuse;
	VECTOR4 ambient;
	VECTOR4 specular;
	float cutoff;
	float attenuation[3];
	float exponent;
	float range;
	int	  used_attribs;

	LIGHT():pos(0,0,0,1),dir(0,0,1),diffuse(1,1,1,1),ambient(0,0,0,1),specular(0,0,0,1),cutoff(180),exponent(0),used_attribs(POINT_LIGHT)
	{
		attenuation[0]=1.0f;
		attenuation[2]=attenuation[1]=0;
	}
};

#endif