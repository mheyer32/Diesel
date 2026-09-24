
/*
This file is part of DXShell
(c) 2002-2026 by Mathias Heyer

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
// FileMananger.cpp: Implementierung der Klasse CFileMananger.
//
//////////////////////////////////////////////////////////////////////

#include "FileManager.h"

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

#include <algorithm>
#include <iostream>

#include <defs.h>
#include <misc/ParseHelper.h>

#include "RamFile.h"
#include "ZipFile.h"

#include <MemoryTracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
ConVar CFileManager::fs_basepath("fs_basepath");
ConCmd CFileManager::cmd_dir("dir", CFileManager::cb_dir);
ConCmd CFileManager::cmd_fs_info("fs_info", CFileManager::cb_fs_info);

using namespace std;

ostream& operator<<(ostream& stream, const std::list<std::string>& filelist)
{
    std::list<std::string>::const_iterator fIt = filelist.begin();
    for (; fIt != filelist.end(); ++fIt) {
        stream << *fIt << endl;
    }
    return stream;
}

CFileManager::CFileManager()
{
    num_pakfiles = 0;

    CPath currentdir;
    getCurrentDirectory(currentdir);
    fs_basepath = currentdir.getString();
}

CFileManager::~CFileManager()
{
    clearPakFileCache();
}

void CFileManager::cachePakFiles(const CPath& directory)
{
    cout << endl << "CFileManager::cachePakFiles() for '" << directory.getString() << "' ..." << endl << endl;

    PATHLIST filelist;

    getFileList(filelist, directory, "*.pk3", false);

    int oldnum_pakfiles = num_pakfiles;
    num_pakfiles += filelist.size();

    pakfiles.resize(num_pakfiles);

    unsigned short pakfile = oldnum_pakfiles;
    CPath          file;

    PATHLIST           zipdir;
    PATHLIST::iterator zfIt;
    PATHITERATOR       fIt = filelist.begin();
    // std::cout<<filelist;
    for (; fIt != filelist.end(); ++fIt) {
        file = directory / (*fIt);
        cout << "    adding packfile  " << file << endl;
        pakfiles[pakfile] = file.getString();

        CZipFile zip;
        if (!zip.setZipFile(file.getString().c_str())) {
            //	cout<<"CFileManager::cachePakFiles() could not open: "<<file.getString()<<endl;
            continue;
        }

        zipdir.clear();
        zip.getFileList(zipdir);

        zfIt = zipdir.begin();

        for (; zfIt != zipdir.end(); ++zfIt) {
            pakcache.add(*zfIt, pakfile);
        }
        pakfile++;
    }

    // cout<<endl<<"resulting pakcache:"<<endl;
    // pakcache.print();
    // cout<<endl;

    num_pakfiles = pakfile;
    cout << endl << "..." << num_pakfiles << " pakfiles" << endl << endl;
}
void CFileManager::clearPakFileCache()
{
    pakcache.clear();
    pakfiles.clear();
    num_pakfiles = 0;
}

bool CFileManager::findFile(const CPath& filename, CPath& fullname)
{
    CFile file;
    if (file.open(filename.getString())) {
        fullname = filename;
        return true;
    }
    if (filename.getType() != CPath::PT_RELATIVE)
        return false;
    CPath temppath;
    //	cout<<"looking in searchpaths...  ";
    PATHITERATOR pIt = searchpaths.begin();
    for (; pIt != searchpaths.end(); ++pIt) {
        //	cout<<(*pIt).getString()<<" ";
        temppath = *pIt / filename;
        if (file.open(temppath.getString())) {
            fullname = temppath;
            //		cout<<" found!"<<endl;
            return true;
        }
    }

    return false;
}
CFile* CFileManager::open(const CPath& Filename, CFile::MODE Mode, bool preferramfiles)
{
    //	cout<<"CFileManager::Open() for "<<Filename<<endl;

    CFile* file = NULL;

    CPath fullname;

    if (findFile(Filename, fullname)) {
        if (preferramfiles) {
            file = new CRamFile();
        } else {
            file = new CFile();
        }
        if (file->open(fullname)) {
            //		cout<<" file found"<<endl;
            return file;
        }
    }

    //	cout<<endl;
    KILLOBJECT(file);

    //	cout<<"looking in pakfile cache:"<<endl;
    unsigned short paknum;
    if (!pakcache.find(Filename, paknum)) {
        // cout<<"file unknown to pakcache"<<endl;
        return NULL;
    }

    CZipFile* zipfile = new CZipFile;

    if (zipfile->setZipFile(pakfiles[paknum].getString())) {
        if (zipfile->open(Filename)) {
            // cout<<" found in pakfile "<<pakfiles[paknum].getString()<<endl;
            return zipfile;
        }
    }

    // cout<<"not found"<<endl;
    KILLOBJECT(zipfile);

    return NULL;
}

void CFileManager::getFileList(PATHLIST& filelist, const CPath& directory, const std::string& filter, bool r_subdirs)
{
    // cout<<"CFileManager::getFileList() for directory: "<<directory.getString()<<endl;
    if (directory.getType() != CPath::PT_RELATIVE)  // absolut path
    {
        CFile::getFileList(filelist, directory, filter, r_subdirs);
        // CFile::getFileList(filelist,directory.getString(),filter,r_subdirs);
    } else {
        char currdir[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, currdir);

        PATHITERATOR pIt = searchpaths.begin();
        for (; pIt != searchpaths.end(); ++pIt) {
            CPath path = (*pIt) / directory;
            // cout<<"searching in "<<path.getString()<<endl;
            BOOL rval = SetCurrentDirectory(path.getString().c_str());
            if (rval == FALSE)
                continue;

            PATHLIST tempdir;
            CFile::getFileList(tempdir, "", filter, r_subdirs);
            // add searchdirectory-pathname
            PATHITERATOR fIt = tempdir.begin();
            for (; fIt != tempdir.end(); ++fIt) {
                filelist.push_front(path / (*fIt));
            }
            SetCurrentDirectory(currdir);
        }
    }

    // I assume the pakfiles won´t change during execution, so I take the directory from the cache, which is
    // much faster either
    //	cout<<"searching in pakcache:"<<endl;

    pakcache.getFileList(filelist, directory.getString(), r_subdirs);
    filelist.sort();
    filelist.unique();

    /*	cout<<"resulting filelist:"<<endl;
        PATHITERATOR pIt=filelist.begin();
        for (;pIt!=filelist.end();++pIt)
        {
            cout<<*pIt<<endl;
        }
        cout<<endl;*/
}

void CFileManager::cb_dir(ConCmd& command, const std::string& args)
{
    cout << std::endl << "Directory: \"" << args << "\"..." << endl << endl;

    PATHLIST dir;
    Instance()->getFileList(dir, args.c_str(), "", false);
    PATHITERATOR fIt = dir.begin();

    for (; fIt != dir.end(); ++fIt) {
        cout << "    " << (*fIt).getString().c_str() << endl;
    }

    cout << endl << "..." << dir.size() << " file(s)" << endl;
}
void CFileManager::cb_fs_info(ConCmd& command, const std::string& args)
{
    cout << endl;
    cout << "searchpaths..." << endl;
    PATHITERATOR pIt = Instance()->searchpaths.begin();
    for (; pIt != Instance()->searchpaths.end(); ++pIt) {
        cout << "    " << (*pIt).getString() << endl;
    }
    cout << Instance()->searchpaths.size() << " searchpaths";

    cout << endl << "pakfiles..." << endl;
    PAKITERATOR pkIt = Instance()->pakfiles.begin();
    for (; pkIt != Instance()->pakfiles.end(); ++pkIt) {
        cout << "    " << (*pkIt) << endl;
    }
    cout << Instance()->pakfiles.size() << " pakfiles" << endl << endl;

    cout << Instance()->pakcache.getNumFiles() << " files cached in pakcache" << endl << endl;
}
void CFileManager::addSearchPath(const CPath& directory)
{
    if (directory.getType() == CPath::PT_RELATIVE) {
        // relative searchpaths will be turned into absolute paths, to not be
        // affected by a change of the working directory
        CPath temppath;
        getCurrentDirectory(temppath);
        temppath /= directory;
        checkAndAddSearchPath(temppath);
    } else {
        checkAndAddSearchPath(directory);
    }
}

void CFileManager::clearSearchPaths()
{
    searchpaths.clear();
}

void CFileManager::checkAndAddSearchPath(const CPath& path)
{
    PATHITERATOR pIt = std::find(searchpaths.begin(), searchpaths.end(), path);
    if (pIt == searchpaths.end()) {
        searchpaths.push_front(path);
        cout << "added searchpath: " << path << endl;
    }
}

void CFileManager::getCurrentDirectory(CPath& directory)
{
    char curr_dir[MAX_PATH];
    ::GetCurrentDirectory(MAX_PATH, curr_dir);
    directory = curr_dir;
}

bool CFileManager::setCurrentDirectory(const CPath& directory)
{
    BOOL rval = ::SetCurrentDirectory(directory.getString().c_str());
    return rval == TRUE;
}
