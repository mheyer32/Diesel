
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
// ZipFile.h: Schnittstelle für die Klasse CZipFile.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZIPFILE_H__826DCABC_7164_4302_A2C7_81CA457DAD95__INCLUDED_)
#define AFX_ZIPFILE_H__826DCABC_7164_4302_A2C7_81CA457DAD95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RamFile.h"
#include "Path.h"

extern "C"
{
#define ZLIB_WINAPI	//muss zu den defines passen, mit denen ZLib gebaut wurde
#define ASMV
#define ASMINF
#include "contrib/minizip/unzip.h"
};

class CZipFile : public CRamFile  
{
public:
	CZipFile();
	virtual ~CZipFile();

	bool setZipFile(const std::string &zipname);

	virtual bool open(const CPath &File, MODE Mode=READ);
	virtual void close();

	void getFileList(PATHLIST &filelist, const CPath &directory="",const std::string &filter="",bool r_subdirs=false);

protected:

	unzFile openzip;
};

#endif // !defined(AFX_ZIPFILE_H__826DCABC_7164_4302_A2C7_81CA457DAD95__INCLUDED_)
