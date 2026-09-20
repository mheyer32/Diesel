
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
// ParseHelper.cpp: Implementierung der Klasse ParseHelper.
//
//////////////////////////////////////////////////////////////////////

#include "ParseHelper.h"

#include <string.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

int ParseHelper::FindComment(char* String, char*& CommentStart, char*& CommentEnd)
{
    char letter;
    CommentStart = String;
    while (CommentStart = strchr(CommentStart, '/')) {
        letter = CommentStart[1];

        if (letter == '/') {
            if (!(CommentEnd = strchr(CommentStart, '\n')))
                return 0;
            return CommentEnd - CommentStart;
        } else if (letter == '*') {
            if (!(CommentEnd = strstr(CommentStart, "*/")))
                return 0;
            CommentEnd += 2;
            return CommentEnd - CommentStart;
        }
        CommentStart += 2;
    }
    return 0;
}

int ParseHelper::FindBracketPair(const char* String, char*& BracketStart, char*& BracketEnd)
{
    int num_brackets, pos;

    pos          = strcspn(String, "{}");
    BracketStart = (char*)String + pos;
    if (*BracketStart != '{')
        return 0;

    BracketEnd   = BracketStart;
    num_brackets = 1;
    while (num_brackets > 0) {
        BracketEnd++;
        pos = strcspn(BracketEnd, "{}");
        BracketEnd += pos;
        if (*BracketEnd == '{') {
            num_brackets++;
        } else if (*BracketEnd == '}') {
            num_brackets--;
        } else
            return 0;
    }

    if (num_brackets == 0) {
        BracketEnd--;    // die beiden Klammern nicht mitzählen!
        BracketStart++;  // erste Klammer skippen
        return BracketEnd - BracketStart;
    };
    return 0;
}

int ParseHelper::Readln(const char* String, char* Line)
{
    int         len;
    const char* lineend;

    if (!(lineend = strchr(String, '\n')))
        return 0;
    len = lineend - String - 1;
    strncpy(Line, String, len);
    Line[len] = 0;
    return len;
}

char* ParseHelper::FindNextLine(const char* String)
{
    char* LineEnd = const_cast<char*>(String);
    if (!(LineEnd = strchr(LineEnd, '\n')))
        return 0;
    LineEnd++;
    return LineEnd;
}

void ParseHelper::RemoveAllComments(char* String)
{
    char *cstart, *cend;
    int   len;

    cend = String;
    while (len = FindComment(cend, cstart, cend)) {
        memset(cstart, ' ', len);
    }
}

void ParseHelper::ReplaceChar(char* String, char c, char r)
{
    while (String = strchr(String, c)) {
        *(String++) = r;
    }
}

inline char ParseHelper::tolower(char c)
{
    if ((c < 0x41) || (c > 0x5A))
        return c;
    else
        return c | 0x20;
}

int ParseHelper::strcmp(const char* string1, const char* string2, unsigned int size)
{
    int count = 0;
    while ((*string1 == *string2) && (*string1 != 0) && count < size)  //&&(*string2!=0) shouldn´t be needed
    {
        string1++;
        string2++;
        count++;
    };
    return count;
}

int ParseHelper::stricmp(const char* string1, const char* string2, unsigned int size)
{
    if (size == -1)
        size = strlen(string1);
    int  count = 0;
    char c1;
    char c2;
    while (*string1 != 0 && count < size) {
        c1 = tolower(*string1);
        c2 = tolower(*string2);
        if (c1 != c2)
            break;
        string1++;
        string2++;
        count++;
    };
    return count;
}
