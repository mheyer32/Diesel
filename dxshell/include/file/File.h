
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
// File.h: Schnittstelle für die Klasse CFile.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILE_H__38B30C00_0195_11D4_BD8A_0000E85E86C1__INCLUDED_)
#define AFX_FILE_H__38B30C00_0195_11D4_BD8A_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <list>
#include <string>

#include "Path.h"

class CFile
{
public:
    enum MODE
    {
        READ,
        WRITE
    };

    CFile();
    virtual ~CFile();

    virtual bool       open(const CPath& File, MODE Mode = READ);
    virtual void       close();
    const std::string& getFilename() const;

    virtual unsigned char  readBYTE();
    virtual void           readSTRING(char* s);
    virtual double         readDOUBLE();
    virtual float          readFLOAT();
    virtual unsigned int   readDWORD();
    virtual unsigned short readWORD();
    virtual void           readVOID(void* ptr, int length);

    virtual void writeBYTE(unsigned char b);
    virtual void writeSTRING(const char* s);
    virtual void writeDOUBLE(double d);
    virtual void writeFLOAT(float f);
    virtual void writeDWORD(unsigned int dw);
    virtual void writeWORD(unsigned short w);
    virtual void writeVOID(const void* ptr, int length);

    virtual bool         setPosition(unsigned int nPos = 0);
    virtual unsigned int getPosition();
    virtual unsigned int getSize();

    static void getFileList(PATHLIST& filelist, const CPath& startdirectory, std::string filter = "",
                            bool recursive = false);

protected:
    static void getFileList_r(PATHLIST& filelist, const CPath& startdirectory, std::string filter = "",
                              bool recursive = false);

    unsigned int m_size;
    FILE*        m_pFile;
    std::string  m_filename;
};

#endif  // !defined(AFX_FILE_H__38B30C00_0195_11D4_BD8A_0000E85E86C1__INCLUDED_)
