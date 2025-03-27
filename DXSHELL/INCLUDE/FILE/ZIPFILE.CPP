
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
// ZipFile.cpp: Implementierung der Klasse CZipFile.
//
//////////////////////////////////////////////////////////////////////

#include "ZipFile.h"
#include <misc/STLHelp.h>
#include <misc/ParseHelper.h>
#include <misc/exception.h>

#include <defs.h>
#include <iostream>

#include <memorytracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CZipFile::CZipFile()
{
	openzip=0;
}

CZipFile::~CZipFile()
{
	if (openzip)
	{
		unzClose(openzip);
		openzip=0;
		close();
	}
}

bool CZipFile::setZipFile(const std::string &zipname)
{
	close(); // close File before closing ZIP
	if (openzip)
	{
		int rval=unzClose(openzip);
	}
	openzip=unzOpen(zipname.c_str());
	return openzip!=NULL;
}

bool CZipFile::open(const CPath &filename,MODE mode)
{
	if (!openzip||mode!=READ) return false;

	close();

	std::string tempfilename(filename);

	STLHelp::replace(tempfilename,'\\','/');

	int rval=unzLocateFile( openzip, tempfilename.c_str(), 0);

	if (rval!=UNZ_OK)
	{
		std::cout<<"CZipFile::Open() -> unzLocateFile() failes"<<std::endl;
		return false;
	}

	if (unzOpenCurrentFile(openzip)!=UNZ_OK)
	{
		std::cout<<"CZipFile::Open() -> unzOpenCurrentFile() failed"<<std::endl;
		return false;
	}

	unz_file_info info;
	rval=unzGetCurrentFileInfo( openzip, &info, NULL, 0, NULL, 0, NULL, 0 );
	
	allocBuffer(info.uncompressed_size);

	rval=unzReadCurrentFile( openzip, buffer, m_size);

	unzCloseCurrentFile(openzip);

	if (rval!=m_size)
	{
		std::cout<<"CZipFile::Open() -> unzReadCurrentFile() failed"<<std::endl;
		return false;
	}
	m_filename=filename;
	return true;
}

void CZipFile::close()
{
	KILLARRAY(buffer);
}

void CZipFile::getFileList(PATHLIST &filelist, const CPath &directory,const std::string &filter, bool r_subdirs)
{
	assert(!r_subdirs); // not supported!

	if (!openzip) return;
	unz_file_info info;

	int rval=unzGoToFirstFile(openzip);

	if (directory.getString().empty() && filter.empty())
	{
		// no specific filter or startdirectory were given, so just enumerate the whole zip
		// FIXME: r_subdirs is not
		char filename[MAX_PATH];
		while (rval==UNZ_OK)
		{
			rval=unzGetCurrentFileInfo( openzip, &info, filename, MAX_PATH, NULL, 0, NULL, 0 );
			if (rval==UNZ_OK)
			{
				filelist.push_back(filename);
			}
			rval=unzGoToNextFile(openzip);
		}
	}
	else
	{

		char filename[MAX_PATH];
		char searchdir[MAX_PATH];
		strcpy(searchdir,directory.getString().c_str());
		ParseHelper::ReplaceChar(searchdir,'\\','/'); // Zip-Format des Pfads
		int searchdirlen=strlen(searchdir);

		while (rval==UNZ_OK)
		{
			rval=unzGetCurrentFileInfo( openzip, &info, filename, MAX_PATH, NULL, 0, NULL, 0 );
			if (rval==UNZ_OK)
			{
				strlwr(filename);
				if (ParseHelper::strcmp(filename,searchdir)==searchdirlen) // stimmt pfad(-anfang) mit gegebener Directory überein?
				{
					// FIXME: should pure directorynames be contained inside the filellist?
					// since we are looking for files mainly, subdirectories are usually not needed
					// but getFileList() is also used for printing out directories, for which it is
					// useful if the user can at lest see subdireytories
					//if (filename[searchdirlen]=='/' && filename[searchdirlen+1]!=0)
					{
						// if the found filename isn´t just the directory we look for, its
						// a file inside it, so add it to filelist
						filelist.push_back(filename+searchdirlen+1);
					}
				}
			}
			rval=unzGoToNextFile(openzip);
		}
	}
}

//extern "C"
//{
#define ASMV
#define ASMINF
#include "contrib/minizip/unzip.c"
//};