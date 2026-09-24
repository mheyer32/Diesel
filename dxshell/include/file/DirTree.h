
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
// DirTree.h: Schnittstelle für die Klasse CDirTree.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRTREE_H__5DAC6D39_223A_41D1_8BB5_5C804720E20D__INCLUDED_)
#define AFX_DIRTREE_H__5DAC6D39_223A_41D1_8BB5_5C804720E20D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <map>
#include <string>
#include <vector>

#include "Path.h"

#define MAXPATH 256

template <typename T>
class CDirTree
{

public:
    CDirTree();
    virtual ~CDirTree();

    bool find(const CPath& filename, T& data);
    void add(const CPath& filename, const T& data);
    void clear();

    void setNodeGranularity(int g);
    void setLeafGranularity(int g);

    inline int getNumFiles() { return dirleafs.size(); };

    /** get all files (with path) that are inside the given directory
    if recursive is true, also files below the given directory will be returned
    if return value is false, the given directory was not found
    */
    void getFileList(PATHLIST& filelist, const CPath& startdirectory, bool recursive = false);

    void print();

protected:
    typedef signed int LINK;  // must be signed!

    void add_r(char* file, const T& data, LINK node);
    void print_r(std::string filename, LINK node);

    void fillList_r(PATHLIST& filelist, const CPath& directory, LINK node, bool recursive);

    // find the node/leaf that filename is saved in
    bool findnode(const CPath& filename, LINK& node);

    LINK getNewNode();
    LINK getNewLeaf();

    typedef std::map<std::string, LINK> DIRNODE;
    typedef DIRNODE::iterator           DIRNODEITERATOR;

    std::vector<DIRNODE> dirnodes;
    std::vector<T>       dirleafs;

    char delimiter;
    int  node_granularity;
    int  leaf_granularity;

    typename std::vector<DIRNODE>::size_type num_dirnodes;
    typename std::vector<T>::size_type       num_dirleafs;
};

#include "dirtree.hh"

#endif  // !defined(AFX_DIRTREE_H__5DAC6D39_223A_41D1_8BB5_5C804720E20D__INCLUDED_)
