#include "d3entity.h"
#include <math/matrix3.h>
#include <iostream>
#include "ASELoader.h"
#include "LWOLoader.h"

using namespace std;

D3Entity::D3Entity(void)
{
}

D3Entity::~D3Entity(void)
{
}

void D3Entity::addKeyValue(const std::string& key, const std::string& value)
{
    keyvalues[key] = value;
}

void D3Entity::parseKeyValues()
{
    KEYVALUEITERATOR kIt = keyvalues.begin();

    for (; kIt != keyvalues.end(); ++kIt) {
        const std::string& key   = kIt->first;
        const std::string& value = kIt->second;
        if (key == "model") {
            setModel(value);
        } else if (key == "origin") {
            m_transform.setTranslation(getVec3(value));
        } else if (key == "rotation") {
            m_transform.setRotation(getRotation(value));
        } else if (key == "name") {
            setName(value);
        } else if (key == "angle") {
            m_transform.setAngles(VECTOR3(0, RAD(atof(value.c_str())), 0));
        }
    }
}

VECTOR3 D3Entity::getVec3(const std::string& text)
{
    VECTOR3 vec;
    int     num_args =
        sscanf(text.c_str(), "%f %f %f", &vec.x, &vec.z, &vec.y);  // automatically convert from doom to std format
    if (num_args != 3)
        cout << "WARNING: D3Entity::getVec3() error while parsing vector in entity: " << getName() << endl;
    vec.z = -vec.z;
    return vec;
}

MATRIX3 D3Entity::getRotation(const std::string& text)
{
    MATRIX3 mat;
    // row-major format
    int num_args = sscanf(text.c_str(), "%f %f %f %f %f %f %f %f %f", &mat.m00, &mat.m01, &mat.m02, &mat.m10, &mat.m11,
                          &mat.m12, &mat.m20, &mat.m21, &mat.m22);
    // int num_args=sscanf(text.c_str(),"%f %f %f %f %f %f %f %f
    // %f",&mat.m00,&mat.m10,&mat.m20,&mat.m01,&mat.m11,&mat.m21,&mat.m02,&mat.m12,&mat.m22);

    if (num_args != 9)
        cout << "WARNING: D3Entity::getRotation() error while parsing rotation in entity: " << getName() << endl;

    // convert desired rotation to our coordinate system
    MATRIX3 mat2;
    mat2[0] = 1;
    mat2[3] = 0;
    mat2[6] = 0;
    mat2[1] = 0;
    mat2[4] = 0;
    mat2[7] = 1;
    mat2[2] = 0;
    mat2[5] = -1;
    mat2[8] = 0;

    MATRIX3 mat3(mat2);
    Transpose3(mat3);

    mat = mat2 * mat * mat3;

    return mat;
}

bool D3Entity::setModel(const std::string& modelname)
{
    CMesh* mesh = CMesh::FindMesh(modelname);

    if (mesh) {
        addMesh(mesh);
        return true;
    }

    CPath modelfile(modelname);

    std::string ext = modelfile.getExtension();
    if (ext == "ase") {
        // cout<<"D3Entity::parseKeyValues() try to load: "<<modelfile<<endl;
        ASELoader           loader;
        CEntity::ENTITYLIST entlist;
        loader.LoadASE(modelfile, entlist);
        addChild(entlist);
        return true;
    } else if (ext == "lwo") {
        // cout<<"D3Entity::parseKeyValues() try to load: "<<modelfile<<endl;
        LWOLoader           loader;
        CEntity::ENTITYLIST entlist;
        loader.LoadLWO(modelfile, entlist);
        addChild(entlist);
        return true;
    }

    cout << "^2D3Entity::setModel() could not find model: " << modelname << endl;
    ;
    return false;
}