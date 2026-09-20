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
// DirTree.cpp: Implementierung der Klasse CDirTree.
//
//////////////////////////////////////////////////////////////////////

#include "DirTree.h"

#include <cassert>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace std;

template<typename T>
CDirTree<T>::CDirTree()
{
	delimiter='\\';
	node_granularity=10;
	leaf_granularity=100;
	
	clear();
}

template<typename T>
CDirTree<T>::~CDirTree()
{

}

template<typename T>
void CDirTree<T>::setNodeGranularity(int g)
{
	assert(g>0);
	node_granularity=g;
}

template<typename T>
void CDirTree<T>::setLeafGranularity(int g)
{
	assert(g>0);
	leaf_granularity=g;
}

template<typename T>
typename CDirTree<T>::LINK CDirTree<T>::getNewNode()
{
	LINK node=num_dirnodes++;
	if (num_dirnodes>=dirnodes.size())
	{
		dirnodes.resize(num_dirnodes+node_granularity);
	}
	return node;
}

template<typename T>
typename CDirTree<T>::LINK CDirTree<T>::getNewLeaf()
{
	LINK leaf=num_dirleafs++;
	if (num_dirleafs>=dirleafs.size())
	{
		dirleafs.resize(num_dirleafs+leaf_granularity);
	}
	return leaf;
}


template<typename T>
void CDirTree<T>::add(const CPath &file,const T &data)
{
char File[1024];
	
	if (file.getString().empty())
	{
		cout << "CDirTree<T>::add() adding empty path"<<endl;
	}
	strcpy(File,file.getString().c_str());

//	std::cout<<"CDirTree<T>::add() adding: "<<File<<std::endl;
	add_r(File,data,0);

}

template<typename T>
void CDirTree<T>::add_r(char* file,const T &data, LINK node)
{
	char *subdir=strchr(file,delimiter);
	
	if (subdir)
	{
		// as long as file is a path with subdirectories
		*subdir=0;
		++subdir;

		// try to find the subdirectory in the current node
		DIRNODEITERATOR fIt=dirnodes[node].find(std::string(file));
		if (fIt==dirnodes[node].end()) // subdir isn´t known already
		{
			// create a new node and recurse
			LINK newnode=getNewNode();
			// this will create a new entry inside the current node that points down to "newnode"
			// and is named like the subdirectory
			(dirnodes[node])[file]=newnode;
			add_r(subdir,data,newnode);
		}
		else
		{
			// subdir is known
			LINK oldnode=node;
			node=(*fIt).second; // its supposed that node (except the root) never gets 0
			if (node>0) 
			{
				// known subdir is a real subdir, no file
				// just recurse, reusing the found node
				add_r(subdir,data,node);
			}
			else
			{
				// known subdir is already contained, but as leaf
				// create a new "real" node and link the old known leaf directly beneath it
				// since the old node already carries the right name, the leaf then contains an empty string
				// FIXME: delete the leaf?
				LINK newnode=getNewNode(); //getNewNode might resize "dirnodes" and therefore turn fIt invalid
				fIt=dirnodes[oldnode].find(std::string(file));
				(*fIt).second=newnode;
				(dirnodes[newnode])[" "]=node;
				add_r(subdir,data,newnode);	
			}
		}
	}
	else // just the filename is left
	{
		LINK leaf;
		DIRNODEITERATOR fIt=dirnodes[node].find(std::string(file));
		if (fIt!=dirnodes[node].end())
		{
			// filename is already known
			leaf=(*fIt).second;
			if (leaf>0)
			{
				// but it is not known as file
				// (happens if the directory itself was added, but files in that directory were already added before)
				// add an empty-named leaf that then carries the data for the directoryname
				*file=0;
				add_r(file,data,leaf);
				return;
			}
			leaf=~leaf;
		}
		else
		{
			// file is not known, create a new leaf 
			leaf=getNewLeaf();
			assert(node<(LINK)dirnodes.size());
			(dirnodes[node])[file]=~leaf;
		}
		//and save the data there
		dirleafs[leaf]=data;
	}
}
template<typename T>
void CDirTree<T>::print()
{
	char filename[MAXPATH];
	filename[0]=0;
	print_r(filename,0);
}

template<typename T>
void CDirTree<T>::print_r(std::string filename, LINK node)
{
	filename+=delimiter;
	std::string filename2;

	LINK son;

	DIRNODEITERATOR fIt=dirnodes[node].begin();
	
	for (;fIt!=dirnodes[node].end();++fIt)
	{
		filename2=filename+(*fIt).first;
		son=(*fIt).second;
		if (son>0)
		{
			print_r(filename2,son);
		}
		else
		{
			std::cout<<filename2<<"   "<<dirleafs[~son]<<std::endl;
		}
	}
}

/** note that node must be set to a valid node-number before calling this! */
template<typename T>
bool CDirTree<T>::findnode(const CPath &filename, LINK &node)
{
	char Filename[MAXPATH];
	char *subdir;
	char *file=Filename;

	strcpy(Filename,filename.getString().c_str());

//	std::cout<<"CDirTree<T>::findnode() looking for: "<<Filename<<std::endl;

	subdir=file;
	while(*file)
	{
		// see, if the remaining filename contains path-pieces
		subdir=strchr(file,delimiter);
		if (subdir) // recurse tree
		{
			*subdir=0; // cut away the following pieces of the path
						// leaving only the first path-part
		}
		
		// try to find the path-part in the current node
		assert(node>=0);
		DIRNODEITERATOR fIt=dirnodes[node].find(std::string(file));
		if (fIt==dirnodes[node].end())
		{
			return false;

		}

		if (subdir==NULL) // just the filename was left
		{
			node=(*fIt).second; // this is the node/leaf of the found path part
			return true;
		}

		LINK newnode=(*fIt).second; // this is the node/leaf of the found path part
		if (newnode>0)
		{
			node=newnode;
			// otherwise the found node was a leaf, containing information about the directory itself
			// we´d have to continue inside the same node
		}
		// found a subdirectory as path-part
		file=subdir+1; // continue with next parts of the path
	}
	return true;
	
}

template<typename T>
bool CDirTree<T>::find(const CPath &filename,T &data)
{
//	cout<<"CDirTree<T>::find()"<<endl;
	LINK node=0;

	if (findnode(filename,node))
	{
		if (node>=0)
		{
	//		cout<<"found a node"<<endl;
			// found a node - this search ended at a subdirectory
			// try to find an empty-string-key right below the node
			DIRNODEITERATOR fIt=dirnodes[node].find(" ");
			if (fIt!=dirnodes[node].end())
			{
				node=(*fIt).second;
				if (node>0) // something went wrong, we found the empty string, but it still contains children
				{
					throw CException("CDirTree<T>::find() found empty string with children");
					//FIXME: throw exception ?
					return false;
				}
			}
			else
			{
				// no data for the searched directory
				return false;
			}
		}
	//	cout<<"found the leaf:"<<~node<<endl;
		data=dirleafs[~node];
		return true;
	}

	return false;
}

template<typename T>
void CDirTree<T>::clear()
{
	dirnodes.clear();
	dirleafs.clear();

	num_dirnodes=1; // root always exists
	num_dirleafs=0;

	dirnodes.resize(node_granularity);
	dirleafs.resize(leaf_granularity);
}

template<typename T>
void CDirTree<T>::fillList_r(PATHLIST &filelist,const CPath &directory,LINK node, bool recursive)
{
	DIRNODEITERATOR fIt=dirnodes[node].begin();
	LINK son;
	CPath filename;
	for (;fIt!=dirnodes[node].end();++fIt)
	{
		son=(*fIt).second;
		filename=directory/(*fIt).first;
/*		if (son>=0)
		{
			filename+=delimiter;
		}*/
		// FIXME: option to not include pure directories ?
		filelist.push_back(filename);
		if (son>0&&recursive)
		{
			fillList_r(filelist,filename,son,recursive);
		}
	}
}

template<typename T>
void CDirTree<T>::getFileList(PATHLIST &filelist,const CPath &startdirectory, bool recursive)
{
	LINK node=0;
//	cout<<"CDirTree<T>::getFileList() for directory "<<startdirectory<<endl;
	if (findnode(startdirectory,node))
	{
		//cout<<"initial node found"<<endl;
		fillList_r(filelist,"",node,recursive);
	}
}