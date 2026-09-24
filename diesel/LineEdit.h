/*
This file is part of Diesel
(c) 2002-2026 by Mathias Heyer
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
#pragma once

#include <string>

class CLineEdit
{
public:
    enum MODE
    {
        REPLACE,
        INSERT
    };

    CLineEdit(void);
    virtual ~CLineEdit(void);

    void               clear();
    void               setText(const std::string& newText);
    const std::string& getText() const { return text; };
    void               insertCharacter(char character);
    void               deleteCharacter();
    int                getSize() const { return text.size(); };

    void setMode(MODE newMode);

    int  getCursorPos() const;
    void setCursorPos(unsigned int newPos);
    void cursorLeft();
    void cursorRight();

    operator const std::string&() { return text; };
    CLineEdit& operator=(const std::string& newText)
    {
        setText(newText);
        return *this;
    };
    CLineEdit& operator=(const char* newText)
    {
        setText(std::string(newText));
        return *this;
    };

protected:
    MODE        mode;
    std::string text;
    int         textpos;
};
