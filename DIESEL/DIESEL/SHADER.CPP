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

#include "Shader.h"

#include <defs.h>
#include <math/matrix3.h>
#include <math/plane.h>
#include <misc/STLHelp.h>

#include "ShaderLoader.h"
#include "TextureManager.h"
#include "camera.h"
#include "vertexarrays.h"

#include <cassert>
#include <iostream>

#include <memorytracker.h>

CShader::SHADERLIST CShader::ShaderList;
CShader::SMARTPTR   CShader::DefaultShader;
int                 CShader::max_shaderId = 0;

ConCmd CShader::shaderlist("shaderlist", CShader::cmd_shaderlist);

using namespace std;

CShader::CShader()
{
    num_passes  = 0;
    num_deformv = 0;
    shaderflags = 0;
    usedarrays  = VERTEXARRAY | INDEXARRAY;  // we need at least vertices :-)

    fogcolor[2] = fogcolor[1] = fogcolor[0] = 0.0f;
    fogdistance                             = 400.0f;

    sort = SORT_OPAQUE;

    cloudheight = 128.0f;

    shaderId = max_shaderId++;
    ShaderList.push_back(this);
}

CShader::~CShader()
{
    ShaderList.remove(this);

    cout << "Shader: " << m_name << " deleted" << endl;
}

// enforce lowercase names
void CShader::setName(const std::string& newname)
{
    char temp[512];
    assert(newname.size() < 512);
    strcpy(temp, newname.c_str());
    _strlwr(temp);
    m_name = temp;
}

// Finding a shader by name (maploading) is the only application for the shaderlist. Maybe a map would be better ?
CShader* CShader::findShader(const std::string& Shadername)
{
    SHADERITERATOR s;
    std::string    shadername(Shadername);

    STLHelp::tolower(shadername);

    for (s = ShaderList.begin(); s != ShaderList.end(); ++s) {
        if ((*s)->m_name == shadername)
            return (*s);
    }
    return NULL;
}

// either find, load or generate this shader
// if no shader is given, the scriptfile is only scanned for shaders which are then cached
// if a shader is given and no specific scriptfile, the shader is looked for in the scriptcache
// and  then loaded
// if the shadername is "*" then all shaders in  the scriptfile will be loaded

CShader* CShader::findOrLoadShader(const std::string& shader, bool genLightmapStage, bool genGenericShader)
{

    CShaderLoader loader;
    CShader*      newshader;

    std::string lowercaseshader = shader;

    STLHelp::tolower(lowercaseshader);

    if (newshader = findShader(lowercaseshader))
        return newshader;

    newshader = loader.loadShader(lowercaseshader);

    if (!newshader && genGenericShader) {
        newshader = genShaderFromTexture(lowercaseshader, genLightmapStage);
    }
    return newshader;
}

void CShader::createDefaultShader()
{
    CTexture* tex = CTextureManager::Instance()->findOrLoadTexture("default.jpg");
    if (!tex) {
        throw CException("CShader::createDefaultShader():  \"default.jpg\" not found!");
    }
    DefaultShader = genShaderFromTexture(tex, false);
    DefaultShader->AddRef();
}

CShader* CShader::genShaderFromTexture(CTexture* texture, bool genLightmapStage)
{
    CShader* newshader;

    newshader = new CShader();

    std::string shadername = texture->getName();
    STLHelp::tolower(shadername);
    newshader->setName(shadername);
    newshader->shaderflags |= SHADER_TAGGED;  // generic build shader

    if (genLightmapStage) {
        CShaderPass lmpass;

        lmpass.setLightmapPass();
        lmpass.setDepthWrite(true);

        newshader->addPass(lmpass);

        CShaderPass diffpass;

        diffpass.setBlendFunc(GL_ZERO, GL_SRC_COLOR);
        diffpass.addTexture(texture);

        newshader->addPass(diffpass);
    }

    else {
        // falls sich hier was ändert, muss auch Load3DS::MaterialChunk() angepasst werden!!

        CShaderPass diffpass;

        diffpass.addTexture(texture);
        diffpass.setDepthWrite(true);

        newshader->addPass(diffpass);
    }

    return newshader;
}

// generate a shader that uses the texturefile 'texfile' as first pass
CShader* CShader::genShaderFromTexture(const std::string& texfile, bool genLightmapStage)
{
    CTexture* texture;
    CShader*  newshader;

    char filename[MAX_PATH];
    strcpy(filename, texfile.c_str());
    char* ext = strrchr(filename, '.');
    if (!ext) {
        strcat(filename, ".tga");
    }

    texture = CTextureManager::Instance()->findOrLoadTexture(filename);
    if (!texture)
        return NULL;

    newshader = genShaderFromTexture(texture, genLightmapStage);

    if (newshader) {
        std::string shadername(texfile);
        STLHelp::tolower(shadername);
        newshader->setName(shadername);
    }

    return newshader;
}

CShader::SHADERLIST CShader::getShaderList()
{
    return ShaderList;
}

void CShader::cmd_shaderlist(ConCmd& command, const std::string& args)
{
    SHADERITERATOR s;

    std::cout << std::endl << "id / name / num_passes / refcount" << std::endl;
    for (s = ShaderList.begin(); s != ShaderList.end(); ++s) {
        CShader* S = (*s);
        std::cout << S->shaderId << " " << S->getName() << " " << S->num_passes << " " << S->getRefCount() << std::endl;
    }
    std::cout << ShaderList.size() << " shaders" << std::endl << std::endl;
}

void CShader::addPass(const CShaderPass& newPass)
{
    assert(num_passes < SHADER_MAX_NUM_PASSES);

    passes[num_passes] = newPass;

    validatePass(num_passes);

    num_passes++;
}

void CShader::validate()
{
    sort       = SORT_NEAREST;
    usedarrays = VERTEXARRAY | INDEXARRAY;

    for (int i = 0; i < num_passes; ++i) {
        validatePass(i);
    }
}

void CShader::validatePass(int passnum)
{
    CShaderPass& pass = passes[passnum];

    usedarrays |= pass.getUsedArrays();

    // if first pass and no blendfunc is used, make it writing depthvalues
    if (!passnum && !(pass.getFlags() & PASS_BLENDFUNC)) {
        pass.setDepthWrite(true);
    }

    // check for non-blended upper stage passes
    if (!(pass.getFlags() & (PASS_BLENDFUNC | PASS_ALPHAFUNC))) {
        if (sort > SORT_OPAQUE)
            sort = SORT_OPAQUE;
        if (passnum) {
            std::cout << "WARNING: shader '" << getName() << "' has no blendfunc in pass " << passnum << std::endl;
        }
    }
    if (pass.getAlphaGen() != ALPHAGEN_NONE) {
        if (sort > SORT_ADDITIVE)
            sort = SORT_ADDITIVE;
    }
    // choose default rgbgen mode
    if (pass.getRGBGen() == RGBGEN_NONE) {
        if (pass.getFlags() & PASS_BLENDFUNC) {
            pass.setRGBGen((pass.getTexEnv() != GL_MODULATE) ? RGBGEN_IDENTITYLIGHTING : RGBGEN_IDENTITY);
        } else {
            pass.setRGBGen(RGBGEN_IDENTITYLIGHTING);
        }
    }

    // if pass needs real lighting, we need to know...
    if (pass.getFlags() & PASS_LIGHTING) {
        shaderflags |= SHADER_NEEDSLIGHTING | SHADER_USENORMALS;
    }

    // spheremapping needs normals to be uploaded to GL
    if (pass.getTCGen() == TCGEN_SPHEREMAP) {
        shaderflags |= SHADER_USENORMALS;
    }

    switch (pass.getTexEnv()) {
    case GL_ADD:
        if (sort > SORT_ADDITIVE + SORT_ONE)
            sort = SORT_ADDITIVE + SORT_ONE;
        break;
    case GL_REPLACE:
        if (sort > SORT_OPAQUE)
            sort = SORT_OPAQUE;
        break;
    case GL_MODULATE:
    case GL_DECAL:
    default:
        if (sort > SORT_ADDITIVE)
            sort = SORT_ADDITIVE;
    }

    if (pass.getFlags() & PASS_ALPHAFUNC)
        sort += SORT_ONE;

    if (passnum && (passes[passnum - 1].getFlags() & PASS_ALPHAFUNC) && pass.getDepthFunc() != GL_EQUAL) {
        pass.setFirstUnit();
    }
}

void CShader::setFlags(int mask, int flags)
{
    int allowedflags = SHADER_NOCULL | SHADER_CULLFRONT | SHADER_POLYGONOFFSET | SHADER_NOMIPMAP | SHADER_NOPICMIP |
                       SHADER_PORTAL | SHADER_TAGGED;
    // dont allow other flags to be set through setFlags()
    assert(!(flags & ~allowedflags));

    shaderflags = (shaderflags & ~mask) | flags;

    if (shaderflags & SHADER_PORTAL) {
        sort = SORT_PORTAL;
    }
}

void CShader::addVertexDeformWave(float div, const CWave& wave)
{
    if (num_deformv >= SHADER_MAX_NUM_DEFORMVERTEX) {
        std::cout << "WARNING: shader '" << getName() << "' tried to add mor than " << SHADER_MAX_NUM_DEFORMVERTEX
                  << " deformVertex" << std::endl;
        return;
    }

    deformv[num_deformv]          = VDEF_WAVE;
    deformv_param[num_deformv][0] = div;
    deformv_wave[num_deformv]     = wave;
    num_deformv++;
    usedarrays |= NORMALARRAY;
}

void CShader::addVertexDeformBulge(float width, float height, float speed)
{
    if (num_deformv >= SHADER_MAX_NUM_DEFORMVERTEX) {
        std::cout << "WARNING: shader '" << getName() << "' tried to add mor than " << SHADER_MAX_NUM_DEFORMVERTEX
                  << " deformVertex" << std::endl;
        return;
    }
    deformv[num_deformv]          = VDEF_BULGE;
    deformv_param[num_deformv][0] = width;
    deformv_param[num_deformv][1] = height;
    deformv_param[num_deformv][2] = speed;

    num_deformv++;
    usedarrays |= NORMALARRAY;
    // FIXME: how top prevent that SORT_DONTBATCH gets overwritten?
    sort |= SORT_DONTBATCH;  // make it uncombineable
}

void CShader::addVertexDeformMove(float x, float y, float z, CWave& wave)
{
    if (num_deformv >= SHADER_MAX_NUM_DEFORMVERTEX) {
        std::cout << "WARNING: shader '" << getName() << "' tried to add more than " << SHADER_MAX_NUM_DEFORMVERTEX
                  << " deformVertex" << std::endl;
        return;
    }
    deformv[num_deformv]          = VDEF_MOVE;
    deformv_param[num_deformv][0] = x;
    deformv_param[num_deformv][1] = y;
    deformv_param[num_deformv][2] = z;
    deformv_wave[num_deformv]     = wave;
    num_deformv++;
}
void CShader::addVertexDeformAutosprite()
{
    if (num_deformv >= SHADER_MAX_NUM_DEFORMVERTEX) {
        std::cout << "WARNING: shader '" << getName() << "' tried to add more than " << SHADER_MAX_NUM_DEFORMVERTEX
                  << " deformVertex" << std::endl;
        return;
    }
    deformv[num_deformv++] = VDEF_AUTOSPRITE;
}

void CShader::addVertexDeformAutosprite2()
{
    if (num_deformv >= SHADER_MAX_NUM_DEFORMVERTEX) {
        std::cout << "WARNING: shader '" << getName() << "' tried to add more than " << SHADER_MAX_NUM_DEFORMVERTEX
                  << " deformVertex" << std::endl;
        return;
    }
    deformv[num_deformv++] = VDEF_AUTOSPRITE2;
}
void CShader::addVertexDeformNormal(float amplitude, float freq)
{
    if (num_deformv >= SHADER_MAX_NUM_DEFORMVERTEX) {
        std::cout << "WARNING: shader '" << getName() << "' tried to add more than " << SHADER_MAX_NUM_DEFORMVERTEX
                  << " deformVertex" << std::endl;
        return;
    }
    deformv[num_deformv]          = VDEF_NORMAL;
    deformv_param[num_deformv][0] = amplitude;
    deformv_param[num_deformv][1] = freq;
    num_deformv++;
    usedarrays |= NORMALARRAY;
}

void CShader::setFogParams(float r, float g, float b, float distance)
{
    shaderflags |= SHADER_FOG;
    fogcolor[0] = r;
    fogcolor[1] = g;
    fogcolor[2] = b;
    fogdistance = distance;
}
void CShader::setSkyParams(CTexture** nearboxtextures, CTexture** farboxtextures, float height)
{
    if (nearboxtextures) {
        for (int i = 0; i < 6; ++i) {
            nearbox[i] = nearboxtextures[i];
        }
    }
    if (farboxtextures) {
        for (int i = 0; i < 6; ++i) {
            farbox[i] = farboxtextures[i];
        }
    }
    cloudheight = height;
    shaderflags |= SHADER_SKY;
}

VERTEXDEFORM CShader::getVertexDeform(int vdef_num)
{
    return deformv[vdef_num];
}