/*
This file is part of Diesel
(c) 2002 by Mathias Heyer
email: sonode@gmx.de

Diesel is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Diesel is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "lineedit.h"

using namespace std;

CLineEdit::CLineEdit(void)
{
    mode = INSERT;
    clear();
}

CLineEdit::~CLineEdit(void)
{
}

void CLineEdit::clear()
{
    text.clear();
    textpos = 0;
}
void CLineEdit::setText(const std::string& newText)
{
    text = newText;
    setCursorPos(textpos);
}

void CLineEdit::insertCharacter(char character)
{
    if (textpos == text.size()) {
        text.push_back(character);
        ++textpos;
        return;
    }
    switch (mode) {
    case REPLACE:
        text[textpos] = character;
        break;
    case INSERT:
        text.insert(textpos, 1, character);
        break;
    }
    ++textpos;
}

void CLineEdit::deleteCharacter()
{
    text.erase(textpos, 1);
    if (textpos > text.size())
        textpos = text.size();
}
int CLineEdit::getCursorPos() const
{
    return textpos;
}
void CLineEdit::setCursorPos(unsigned int newPos)
{
    if (newPos >= text.size()) {
        textpos = text.size();
        return;
    }
    textpos = newPos;
}
void CLineEdit::cursorLeft()
{
    if (textpos)
        --textpos;
}
void CLineEdit::cursorRight()
{
    if (textpos < text.size())
        ++textpos;
}