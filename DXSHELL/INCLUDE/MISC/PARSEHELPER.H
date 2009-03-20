
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
// ParseHelper.h: Schnittstelle für die Klasse ParseHelper.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSEHELPER_H__1055CA9F_685D_475A_AA3C_E0BD1753BF7C__INCLUDED_)
#define AFX_PARSEHELPER_H__1055CA9F_685D_475A_AA3C_E0BD1753BF7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace ParseHelper  
{
	// replace all occurances of c with r in String
	void ReplaceChar(char *String, char c,char r);
	// replace all comments (c/c++) in STring with spaces
	void RemoveAllComments(char *String);

	// try to find the next end-of-line, return the pointer to the first character of the next line
	char* FindNextLine(const char *String);

	// try to find the next pair of curlies {}, return the pointers of opening curly +1 and closing curly -1
	// the return value is the size of the string inbetween those pointers
	int FindBracketPair(const char *String,char* &BracketStart,char* &BracketEnd);

	// try to find the next comment in the string, return its start and end
	// the return value is the length of the string
	int FindComment(char *String,char* &CommentStart,char* &CommentEnd);

	// copy String to Line until the next end of line
	int Readln(const char *String,char *Line);

	// compare up to "size" characters of both strings (case-sensitive)
	// returns number of equal characters
	int strcmp(const char*,const char*,unsigned int  size=-1);
	// compare up to "size" characters of both strings (case-insensitive)
	// returns number of equal characters 
	int stricmp(const char*,const char*,unsigned int  size=-1);

	// return lower-case version of c
	char tolower(char c);
};	

#endif // !defined(AFX_PARSEHELPER_H__1055CA9F_685D_475A_AA3C_E0BD1753BF7C__INCLUDED_)
