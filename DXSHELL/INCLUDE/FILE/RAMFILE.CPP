
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
// RamFile.cpp: Implementierung der Klasse CRamFile.
//
//////////////////////////////////////////////////////////////////////

#include <defs.h>
#include <misc/exception.h>
#include "RamFile.h"

#include <memorytracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

template <typename _T>
inline _T readFile(void* buffer)
{
    _T var;
    var = *(_T*)buffer;
    return var;
}

CRamFile::CRamFile()
{
    buffer   = NULL;
    position = 0;
}

CRamFile::~CRamFile()
{
    close();
}

bool CRamFile::open(const CPath& File, MODE mode)
{
    if (mode != READ)
        throw CException("write mode not allowed with CRamFile");
    if (!CFile::open(File, mode))
        return false;

    allocBuffer(m_size);

    CFile::readVOID((void*)buffer, m_size);

    position = 0;
    return true;
}

void CRamFile::close()
{
    CFile::close();
    KILLARRAY(buffer)
    position = 0;
}

inline void CRamFile::checkPosition()
{
    if (position >= m_size)
        throw CException("CRamFile::checkPosition() read after end of file: ", m_filename);
}
unsigned char CRamFile::readBYTE()
{
    checkPosition();
    unsigned char b;
    b = readFile<unsigned char>(&buffer[position]);
    position += sizeof(unsigned char);
    return b;
}

double CRamFile::readDOUBLE()
{
    checkPosition();
    double b;
    b = readFile<double>(&buffer[position]);
    position += sizeof(double);
    return b;
}
float CRamFile::readFLOAT()
{
    checkPosition();
    float b;
    b = readFile<float>(&buffer[position]);
    position += sizeof(float);
    return b;
}
unsigned int CRamFile::readDWORD()
{
    checkPosition();
    unsigned int b;
    b = readFile<unsigned int>(&buffer[position]);
    position += sizeof(unsigned int);
    return b;
}
unsigned short CRamFile::readWORD()
{
    checkPosition();
    unsigned short b;
    b = readFile<unsigned short>(&buffer[position]);
    position += sizeof(unsigned short);
    return b;
}

void CRamFile::readSTRING(char* string)
{
    assert(string);
    checkPosition();

    int length = 0;
    do {
        string[length] = buffer[position + length];
    } while (string[length++] != 0);
    position += length;
}

void CRamFile::readVOID(void* ptr, int length)
{
    if (!length)
        return;
    assert(ptr);
    checkPosition();

    memcpy(ptr, buffer + position, length);
    position += length;
}

bool CRamFile::setPosition(unsigned int nPos)
{
    if (nPos < m_size) {
        position = nPos;
        return true;
    }
    return false;
}

unsigned int CRamFile::getPosition()
{
    return position;
}

void CRamFile::allocBuffer(unsigned int num_bytes)
{
    KILLARRAY(buffer);
    m_size = num_bytes;
    buffer = new unsigned char[num_bytes];
}

void* CRamFile::takeBuffer()
{
    void* buffer2 = buffer;
    buffer        = NULL;
    close();
    return buffer2;
}