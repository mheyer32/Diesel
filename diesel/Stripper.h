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
#define STRIPOPT_SORTONCE 0x00000001
#define STRIPOPT_SORTEVERYTIME 0x00000002

// FIXME: this must match the define in Renderer
//#ifndef INDEX
typedef unsigned short INDEX;
//#endif

extern void ArrangeForStrips(INDEX* Indizes, int num_Indizes, int Options = 0);