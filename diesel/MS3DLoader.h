#pragma once

#include <file/Path.h>
#include <misc/Lexer.h>

#include "MS3DAnimation.h"
#include "MS3DModel.h"

struct VECTOR3;

class MS3DLoader
{
public:
    MS3DLoader(void);
    ~MS3DLoader(void);

    MS3DModel*     loadMS3DModel_ASCII(const CPath& Filename);
    MS3DAnimation* loadMS3DAnimation_ASCII(const CPath& Filename);

protected:
    void readMeshes(MS3DModel* msdata);
    void readMaterials(MS3DModel* msdata);
    void readJoints(MS3DModel* msdata);
    void readKeyframes(MS3DAnimation* anim);

    void readColor(float color[4]);

    void readVector3(VECTOR3& vec);

    Lexer lex;
};
