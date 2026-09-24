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
#include "Entity.h"

#include <map>
#include <string>

class D3Entity : public CEntity
{
public:
    typedef std::list<D3Entity*>               D3ENTITYLIST;
    typedef D3ENTITYLIST::iterator             D3ENTITYITERATOR;
    typedef std::list<SmartPointer<D3Entity> > D3ENTSPTRLIST;
    typedef D3ENTSPTRLIST::iterator            D3ENTSPTRITERATOR;

    D3Entity(void);
    virtual ~D3Entity(void);

    void addKeyValue(const std::string& key, const std::string& value);

    void parseKeyValues();

protected:
    VECTOR3 getVec3(const std::string& text);
    MATRIX3 getRotation(const std::string& text);

    bool setModel(const std::string& modelname);

    typedef std::map<std::string, std::string> KEYVALUEMAP;
    typedef KEYVALUEMAP::iterator              KEYVALUEITERATOR;

    KEYVALUEMAP keyvalues;
};
