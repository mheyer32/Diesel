
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
// streambuf_history.h: Schnittstelle für die Klasse streambuf_history.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STREAMBUF_HISTORY_H__443C2561_1D7A_4349_AD2C_C76FAA97EFCD__INCLUDED_)
#define AFX_STREAMBUF_HISTORY_H__443C2561_1D7A_4349_AD2C_C76FAA97EFCD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <fstream>
#include <string>
#include <strstream>

#include <misc/History.h>

class streambuf_history : public std::strstreambuf, public CHistory<std::string>
{
public:
    streambuf_history(int buflen = 0);
    virtual ~streambuf_history();

    void enableLogging(bool enable, const std::string& logfilename = "logfile.log");

protected:
    virtual int sync();

    std::ofstream logfile;
    bool          logging_enabled;
};

#endif  // !defined(AFX_STREAMBUF_HISTORY_H__443C2561_1D7A_4349_AD2C_C76FAA97EFCD__INCLUDED_)
