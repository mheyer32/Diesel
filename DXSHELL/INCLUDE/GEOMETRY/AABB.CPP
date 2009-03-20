
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
#include "AABB.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>

void DrawBBox(const BBOX &Box)
{
VECTOR3 v2,v3,v4,v5,v6,v8;

	v2=v3=v4=Box.min;
	v5=v6=v8=Box.max;
	v2.x=v3.x=Box.max.x;
	v3.y=v4.y=Box.max.y;
	v5.x=v8.x=Box.min.x;
	v5.y=v6.y=Box.min.y;
	
	glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT );
	glColor3ub(255,0,0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glBegin(GL_LINE_STRIP);
		glVertex3fv((float*)&Box.min);
		glVertex3fv((float*)&v2);
		glVertex3fv((float*)&v3);
		glVertex3fv((float*)&Box.max);
		glVertex3fv((float*)&v6);
		glVertex3fv((float*)&v5);
		glVertex3fv((float*)&v8);
		glVertex3fv((float*)&v4);
		glVertex3fv((float*)&Box.min);
		glVertex3fv((float*)&v5);
		glVertex3fv((float*)&v8);
		glVertex3fv((float*)&Box.max);
		glVertex3fv((float*)&v6);
		glVertex3fv((float*)&v2);
		glVertex3fv((float*)&v3);
		glVertex3fv((float*)&v4);
	glEnd();

	glPopAttrib();
}

bool checkBBox(const BBOX &Box)
{
	if (Box.min.x>Box.max.x	|| Box.min.y>Box.max.y ||Box.min.z>Box.max.z) return false;
	return true;
}

std::ostream& operator << (std::ostream &stream, const BBOX &box)
{
	stream<<'['<<box.min<<' '<<box.max<<']';
	return stream;
}


