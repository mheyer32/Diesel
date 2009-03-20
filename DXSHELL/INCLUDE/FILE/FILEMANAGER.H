
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
// FileMananger.h: Schnittstelle für die Klasse CFileMananger.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEMANAGER_H__88C4C923_5CA4_4E22_98C7_783678726748__INCLUDED_)
#define AFX_FILEMANAGER_H__88C4C923_5CA4_4E22_98C7_783678726748__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning (disable: 4786)  // STL-überlängen-Meldungen unterdruecken
#include <map>
#include <string>
#include <vector>
#include <adt/ConCmd.h>
#include <adt/ConVar.h>
#include <adt/Singleton.h>

#include "file/file.h"
#include "Path.h"
#include "dirtree.h"

class CFileManager : public Singleton<CFileManager>
{

DECLARE_SINGLETON(CFileManager)

public:

	
	void cachePakFiles(const CPath &directory);
	void clearPakFileCache();

	void addSearchPath(const CPath &directory);
	void clearSearchPaths();

	void getCurrentDirectory(CPath& directory);
	bool setCurrentDirectory(const CPath& directory);

	/** search "filename" in all searchpaths, if found return full path in "fullname" */
	bool findFile(const CPath &filename, CPath &fullname);

	 CFile* open(const CPath& Filename,CFile::MODE Mode=CFile::READ,bool preferramfiles=false);

	void getFileList(PATHLIST &filelist,const CPath &directory,const std::string &filter="",bool r_subdirs=true);

	static ConVar   fs_basepath;

protected:

	CFileManager();
 	virtual ~CFileManager();

	void checkAndAddSearchPath(const CPath &path);

	std::vector<CPath> pakfiles;
	typedef std::vector<CPath>::iterator PAKITERATOR;

	PATHLIST searchpaths;

	int num_pakfiles;
	CDirTree<unsigned short> pakcache;

	static ConCmd cmd_dir;
	static ConCmd cmd_fs_info;
	static void cb_dir(ConCmd &command,const std::string &args);
	static void cb_fs_info(ConCmd &command,const std::string &args);
};

#endif // !defined(AFX_FILEMANANGER_H__88C4C923_5CA4_4E22_98C7_783678726748__INCLUDED_)
