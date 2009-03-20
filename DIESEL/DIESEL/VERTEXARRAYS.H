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
#define NO_ARRAY		0
#define VERTEXARRAY		(1<<0)
#define NORMALARRAY		(1<<1)
#define TEXCOORDARRAY	(1<<2)
#define INDEXARRAY		(1<<3)
#define COLORARRAY		(1<<4)
#define LMCOORDARRAY	(1<<5)
#define ALL_ARRAYS		VERTEXARRAY|NORMALARRAY|TEXCOORDARRAY|INDEXARRAY|COLORARRAY|LMCOORDARRAY 