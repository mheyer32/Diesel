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
