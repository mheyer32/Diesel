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
#include "FileRequester.h"

#include <commdlg.h>

CFileRequester::CFileRequester(void)
{
}

CFileRequester::~CFileRequester(void)
{
}

bool CFileRequester::openRequester(std::string& filename)
{
    CHAR file[MAX_PATH], directory[MAX_PATH];
    CHAR path[MAX_PATH];

    ZeroMemory(path, sizeof(directory));
    ZeroMemory(file, sizeof(directory));

    GetCurrentDirectory(MAX_PATH, directory);

    OPENFILENAME ofn = {
        sizeof(OPENFILENAME),  //        lStructSize;
        NULL,                  //         hwndOwner;
        NULL,                  //     hInstance;
        NULL,                  //      lpstrFilter;
        NULL,                  //        lpstrCustomFilter;
        0,                     //         nMaxCustFilter;
        0,                     //        nFilterIndex;
        file,                  //        lpstrFile;
        1024,                  //    nMaxFile;
        NULL,                  //        lpstrFileTitle;
        0,                     //         nMaxFileTitle;
        directory,             //       lpstrInitialDir;
        NULL,                  //       lpstrTitle;
        0,                     //         Flags;
        0,                     //          nFileOffset;
        0,                     //          nFileExtension;
        NULL,                  //       lpstrDefExt;
        0,                     //         lCustData;
        NULL,                  // lpfnHook;
        NULL                   //       lpTemplateName;
    };
    if (GetOpenFileName(&ofn) == FALSE)
        return false;

    SetCurrentDirectory(directory);
    filename = file;
    return true;
}