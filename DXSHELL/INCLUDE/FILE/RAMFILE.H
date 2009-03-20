
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
// RamFile.h: Schnittstelle für die Klasse CRamFile.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAMFILE_H__F7067F5E_88D8_4D5A_80DB_1CB36F323BE7__INCLUDED_)
#define AFX_RAMFILE_H__F7067F5E_88D8_4D5A_80DB_1CB36F323BE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "File.h"
#include <cassert>

class CRamFile : public CFile  
{
public:

	CRamFile();
	virtual ~CRamFile();


	virtual void close();
	virtual bool open(const CPath &File, MODE Mode=READ);

	virtual unsigned char readBYTE();
	virtual void readSTRING(char *s);
	virtual double readDOUBLE();
	virtual float readFLOAT();
	virtual unsigned int readDWORD();
	virtual unsigned short readWORD();
	virtual void readVOID(void *ptr,int length);

	virtual bool setPosition(unsigned int nPos=0);
	virtual unsigned int getPosition();

	void* takeBuffer();

protected:

	void allocBuffer(unsigned int num_bytes);
	void checkPosition();

	unsigned int position;
	unsigned char *buffer;
};

#endif // !defined(AFX_RAMFILE_H__F7067F5E_88D8_4D5A_80DB_1CB36F323BE7__INCLUDED_)
