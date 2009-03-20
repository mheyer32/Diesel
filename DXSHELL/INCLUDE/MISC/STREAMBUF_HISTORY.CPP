
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
// streambuf_history.cpp: Implementierung der Klasse streambuf_history.
//
//////////////////////////////////////////////////////////////////////

#include "streambuf_history.h"
#include <iostream>


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////


streambuf_history::streambuf_history(int buflen) : std::strstreambuf(buflen)
{
	logging_enabled=false;
}

streambuf_history::~streambuf_history()
{
	logfile.close();
}

void streambuf_history::enableLogging(bool enable,const std::string &logfilename)
{
	logging_enabled=enable;
	if (logging_enabled)
	{
		logfile.open(logfilename.c_str());
	}
	else
	{
		logfile.close();
	}
}

int streambuf_history::sync()
{
	std::string text(pbase(),pcount());

	add(text); // add string to history

	if (logging_enabled)
	{
		//std::clog<<text;
		logfile<<text<<std::flush;
	}

	seekpos(0);

	return 0;
}
//--------------------------------------------------------------------------------------------------------