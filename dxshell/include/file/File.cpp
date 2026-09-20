
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
// File.cpp: Implementierung der Klasse CFile.
//
//////////////////////////////////////////////////////////////////////

#include "File.h"

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

#include <defs.h>
#include <misc/Exception.h>

#include <cassert>
#include <iostream>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

template <typename _T>
inline void writeFile(FILE* file, const _T& val)
{
    assert(file);
    assert(!feof(file));

    fwrite(&val, sizeof(_T), 1, file);

    assert(!ferror(file));
}

template <typename _T>
inline _T readFile(FILE* file)
{
    assert(file);
    assert(!feof(file));
    _T var;
    fread(&var, sizeof(_T), 1, file);
    assert(!ferror(file));
    return var;
}

CFile::CFile()
{
    m_pFile = NULL;
    m_size  = 0;
}

CFile::~CFile()
{
    close();
}

bool CFile::open(const CPath& File, MODE Mode)
{
    m_filename = File;

    switch (Mode) {
    case READ:

        m_pFile = fopen(File.getString().c_str(), "rb");
        if (!m_pFile) {
            // std::cout<<"CFile::Open() Konnte File nicht öffnen - READ:\n"<<m_filename<<std::endl;
            return false;
        }
        break;
    case WRITE:
        m_pFile = fopen(File.getString().c_str(), "wb");
        if (!m_pFile)  // FIXME: Nachfragen, ob überschrieben werden soll
        {
            std::cout << "CFile::Open() File already exists - WRITE\n" << m_filename << std::endl;
            return false;
        }
    }

    if (Mode == READ) {
        fseek(m_pFile, 0, SEEK_END);
        m_size = ftell(m_pFile);
        rewind(m_pFile);
    } else {
        m_size = 0;
    }

    return true;
}

void CFile::close()
{
    if (m_pFile != 0) {
        fclose(m_pFile);
        m_pFile = 0;
        m_size  = 0;
    }
}

const std::string& CFile::getFilename() const
{
    return m_filename;
}

bool CFile::setPosition(unsigned int nPos)
{
    return !fseek(m_pFile, nPos, SEEK_SET);
}

unsigned char CFile::readBYTE()
{
    return readFile<unsigned char>(m_pFile);
}

unsigned short CFile::readWORD()
{
    return readFile<unsigned short>(m_pFile);
}

unsigned int CFile::readDWORD()
{
    return readFile<unsigned int>(m_pFile);
}

float CFile::readFLOAT()
{
    return readFile<float>(m_pFile);
}

double CFile::readDOUBLE()
{
    return readFile<double>(m_pFile);
}

void CFile::readSTRING(char* String)
{
    int length = 0;
    do {
        String[length] = readBYTE();
    } while (String[length++] != 0);
}

void CFile::writeBYTE(unsigned char b)
{
    writeFile(m_pFile, b);
}

void CFile::writeWORD(unsigned short w)
{
    writeFile(m_pFile, w);
}

void CFile::writeDWORD(unsigned int dw)
{
    writeFile(m_pFile, dw);
}

void CFile::writeFLOAT(float f)
{
    writeFile(m_pFile, f);
}

void CFile::writeDOUBLE(double d)
{
    writeFile(m_pFile, d);
}

void CFile::writeSTRING(const char* s)
{
    int length = strlen(s) + 1;
    writeVOID((void*)s, length);
}

void CFile::readVOID(void* ptr, int length)
{
    assert(m_pFile);
    assert(!feof(m_pFile));
    fread(ptr, 1, length, m_pFile);  // read n bytes
    assert(!ferror(m_pFile));
}

void CFile::writeVOID(const void* ptr, int length)
{
    assert(m_pFile);
    fwrite(ptr, 1, length, m_pFile);
    assert(!ferror(m_pFile));
}

unsigned int CFile::getPosition()
{
    return ftell(m_pFile);
}

unsigned int CFile::getSize()
{
    return m_size;
}

void CFile::getFileList(PATHLIST& filelist, const CPath& startdirectory, std::string filter, bool recursive)
{
    char current_dir[MAX_PATH];

    GetCurrentDirectory(MAX_PATH, current_dir);

    if ((!startdirectory.getString().empty()) && SetCurrentDirectory(startdirectory.getString().c_str()) == FALSE) {
        // can´t change into the directory
        return;
    };

    getFileList_r(filelist, "", filter, recursive);

    SetCurrentDirectory(current_dir);
}

void CFile::getFileList_r(PATHLIST& filelist, const CPath& startdirectory, std::string filter, bool recursive)
{
    // build correct search-string for FindFirst/FindNext
    CPath tempfilter(startdirectory);

    if (!filter.empty()) {
        tempfilter /= filter;
    } else {
        tempfilter /= "*";
    }

    WIN32_FIND_DATA find;

    HANDLE hfind = FindFirstFile(tempfilter.getString().c_str(), &find);
    CPath  newfile;
    if (hfind != INVALID_HANDLE_VALUE) {
        do {

            if ((find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                if (!EQ(find.cFileName, ".") && !EQ(find.cFileName, ".."))  // we don´t want these
                {
                    newfile = startdirectory;
                    newfile /= find.cFileName;
                    // FIXME: should pure directorynames be contained inside the filellist?
                    filelist.push_back(newfile);
                    if (recursive) {
                        getFileList_r(filelist, newfile, filter, recursive);
                    }
                }
            } else {
                newfile = startdirectory;
                newfile /= find.cFileName;
                filelist.push_back(newfile);
            }

        } while (FindNextFile(hfind, &find));

        FindClose(hfind);
    }
}
