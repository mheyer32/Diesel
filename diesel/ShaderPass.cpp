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
#include "ShaderPass.h"
#include <opengl/OpenGL.h>
#include "Renderer.h"  // for some CVars

#include <cassert>
#include <iostream>

CShaderPass::CShaderPass(void)
{
    passflags  = 0;
    usedarrays = 0;

    blendsrc  = GL_ONE;
    blenddest = GL_ZERO;
    texenv    = GL_REPLACE;

    rgbgen   = RGBGEN_IDENTITY;
    alphagen = ALPHAGEN_IDENTITY;

    alphafuncref = 0;

    tcgen = TCGEN_NONE;

    anim_numframes = 0;
    anim_fps       = 10;

    tcmod_num      = 0;
    tcmod_scale[0] = 1.0f;
    tcmod_scale[1] = 1.0f;
}

CShaderPass::~CShaderPass(void)
{
}

void CShaderPass::addTexture(CTexture* texture)
{
    assert(anim_numframes < SHADER_MAX_NUM_ANIM_FRAMES);
    anim_frames[anim_numframes++] = texture;
    if (anim_numframes > 1)
        passflags |= PASS_ANIMMAP;
    usedarrays |= TEXCOORDARRAY;
}

void CShaderPass::setFPS(float fps)
{
    anim_fps = fps;
}

void CShaderPass::setBlendFunc(int src, int dest)
{
    passflags &= ~(PASS_DEPTHWRITE | PASS_TEXENV);  // blended passes usually don´t write zvalues
    passflags |= PASS_BLENDFUNC;
    texenv = -1;

    blendsrc  = src;
    blenddest = dest;

    if (src == GL_ONE && dest == GL_ONE) {
        if ((COpenGL::Instance()->getCaps() & GLCAPS_TEXTURE_ENV_ADD) && (int)Renderer::r_ext_texture_env_add) {
            passflags |= PASS_TEXENV;
            texenv = GL_ADD;
        }
    } else if (src == GL_ONE && dest == GL_ZERO) {
        passflags |= PASS_TEXENV;
        texenv = GL_REPLACE;
        passflags &= ~PASS_BLENDFUNC;
        // FIXME: if (!passnum) passflags|=PASS_DEPTHWRITE;
    } else if ((src == GL_ZERO && dest == GL_SRC_COLOR) || (src == GL_DST_COLOR && dest == GL_ZERO)) {
        blendsrc  = GL_DST_COLOR;  // in einheitliche Reihenfolge bringen
        blenddest = GL_ZERO;       // FIXME: ob die Reihenfolge irgendwelche Auswirkungen in der 2. Texturestage hat?
        passflags |= PASS_TEXENV;
        texenv = GL_MODULATE;
    } else if (src == GL_SRC_ALPHA && dest == GL_ONE_MINUS_SRC_ALPHA) {
        passflags |= PASS_TEXENV;
        texenv = GL_DECAL;
    }
}

void CShaderPass::setAlphaFunc(int func, float alpharef)
{
    alphafunc    = func;
    alphafuncref = alpharef;
    passflags |= PASS_ALPHAFUNC;
}

void CShaderPass::setDepthFunc(int depthf)
{
    depthfunc = depthf;
    if (depthfunc != GL_LEQUAL) {
        passflags |= PASS_DEPTHFUNC;
    }
}

void CShaderPass::setRGBGen(RGBGENFUNC rgbgf)
{
    assert(rgbgf != RGBGEN_CONST);
    assert(rgbgf != RGBGEN_WAVE);
    rgbgen = rgbgf;

    switch (rgbgen) {
    case RGBGEN_MATERIAL: {
        for (int t = 0; t < anim_numframes; ++t) {
            anim_frames[t]->setTexParameter(TEX_USEMATERIAL, TEX_USEMATERIAL);
        }
    }
    case RGBGEN_LIGHTINGDIFFUSE:
        passflags |= PASS_LIGHTING;
        usedarrays |= NORMALARRAY;
        break;
    case RGBGEN_VERTEX:
    case RGBGEN_EXACTVERTEX:
    case RGBGEN_ONEMINUSVERTEX:
        usedarrays |= COLORARRAY;
        break;
    }
}

void CShaderPass::setRGBGenWave(const CWave& rgbwave)
{
    rgbgen_func = rgbwave;
    rgbgen      = RGBGEN_WAVE;
}

void CShaderPass::setRGBGenConst(float r, float g, float b)
{
    rgbgen          = RGBGEN_CONST;
    rgbgen_const[0] = r;
    rgbgen_const[1] = g;
    rgbgen_const[2] = b;
}

void CShaderPass::setAlphaGen(ALPHAGENFUNC alphag)
{
    assert(alphagen != ALPHAGEN_WAVE);
    assert(alphagen != ALPHAGEN_PORTAL);
    alphagen = alphag;

    switch (alphagen) {
    case ALPHAGEN_LIGHTINGDIFFUSE:
        passflags |= PASS_LIGHTING;
        // intentional fall through
    case ALPHAGEN_LIGHTINGSPECULAR:
        usedarrays |= NORMALARRAY;
        break;
    case ALPHAGEN_VERTEX:
    case ALPHAGEN_ONEMINUSVERTEX:
        usedarrays |= COLORARRAY;
        break;
    }
}

void CShaderPass::setAlphaGenWave(const CWave& alphawave)
{
    alphagen      = ALPHAGEN_WAVE;
    alphagen_func = alphawave;
}

void CShaderPass::setAlphaGenPortal(float portalrange)
{
    alphagen             = ALPHAGEN_PORTAL;
    alphagen_portalrange = portalrange;
}
void CShaderPass::addTCModScale(float s_scale, float t_scale)  // scale s and t
{
    if (tcmod_num >= SHADER_MAX_NUM_TCMOD) {
        std::cout << "WARNING: too many tcMod stages in shaderpass" << std::endl;
    }
    tcmod_order[tcmod_num++] = TCMOD_SCALE;
    tcmod_scale[0]           = s_scale;
    tcmod_scale[1]           = t_scale;
}
void CShaderPass::addTCModRotate(float speed)
{
    if (tcmod_num >= SHADER_MAX_NUM_TCMOD) {
        std::cout << "WARNING: too many tcMod stages in shaderpass" << std::endl;
    }
    tcmod_order[tcmod_num++] = TCMOD_ROTATE;
    tcmod_rotate             = speed;
}
void CShaderPass::addTCModScroll(float s_speed, float t_speed)
{
    if (tcmod_num >= SHADER_MAX_NUM_TCMOD) {
        std::cout << "WARNING: too many tcMod stages in shaderpass" << std::endl;
    }
    tcmod_order[tcmod_num++] = TCMOD_SCROLL;
    tcmod_scroll[0]          = s_speed;
    tcmod_scroll[1]          = t_speed;
}
void CShaderPass::addTCModTransform(float m00, float m01, float m10, float m11, float t0, float t1)
{
    if (tcmod_num >= SHADER_MAX_NUM_TCMOD) {
        std::cout << "WARNING: too many tcMod stages in shaderpass" << std::endl;
    }
    tcmod_order[tcmod_num++] = TCMOD_TRANSFORM;
    tcmod_transform[0]       = m00;
    tcmod_transform[1]       = m01;
    tcmod_transform[2]       = m10;
    tcmod_transform[3]       = m11;
    tcmod_transform[4]       = t0;
    tcmod_transform[5]       = t1;
}
void CShaderPass::addTCModTurb(float base, float amplitude, float phase, float freq)
{
    if (tcmod_num >= SHADER_MAX_NUM_TCMOD) {
        std::cout << "WARNING: too many tcMod stages in shaderpass" << std::endl;
    }
    tcmod_order[tcmod_num++] = TCMOD_TURB;
    tcmod_turb[0]            = base;
    tcmod_turb[1]            = amplitude;
    tcmod_turb[2]            = phase;
    tcmod_turb[3]            = freq;
}

void CShaderPass::addTCModStretch(const CWave& stretch)
{
    if (tcmod_num >= SHADER_MAX_NUM_TCMOD) {
        std::cout << "WARNING: too many tcMod stages in shaderpass" << std::endl;
    }
    tcmod_order[tcmod_num++] = TCMOD_STRETCH;
    tcmod_stretch            = stretch;
}

void CShaderPass::setTCGen(TCGENFUNC tcg)
{
    assert(tcg != TCGEN_VECTOR);
    tcgen = tcg;
    if (tcgen == TCGEN_NONE)
        return;

    usedarrays &= ~(TEXCOORDARRAY | LMCOORDARRAY);  // all other modes produce texturecoords themselves
    switch (tcgen) {
    case TCGEN_LIGHTMAP:
        usedarrays |= LMCOORDARRAY;
        break;
    case TCGEN_SPHEREMAP:
        passflags |= PASS_GLTCGEN;  // glTexGen() will be used
                                    // intentionally fall through
    case TCGEN_ENVIRONMENT:
        usedarrays |= NORMALARRAY;
        break;
    }
}
void CShaderPass::setTCGenVector(const VECTOR3& s, const VECTOR3& t)
{
    tcgen = TCGEN_VECTOR;
    usedarrays &= ~(TEXCOORDARRAY | LMCOORDARRAY);  // all other modes produce texturecoords themselves
    passflags |= PASS_GLTCGEN;                      // glTexGen() will be used
    tcgen_vector[0] = s;
    tcgen_vector[1] = t;
}

void CShaderPass::setDetailPass()
{
    passflags |= PASS_DETAIL;
}

void CShaderPass::setLightmapPass()
{
    passflags |= PASS_LIGHTMAP;
    if (tcgen == TCGEN_NONE) {
        setTCGen(TCGEN_LIGHTMAP);
    }
}

void CShaderPass::setDepthWrite(bool write)
{
    if (write)
        passflags |= PASS_DEPTHWRITE;
    else
        passflags &= ~PASS_DEPTHWRITE;
}
void CShaderPass::setDepthTest(bool test)
{
    if (test)
        passflags &= ~PASS_NODEPTHTEST;
    else
        passflags |= PASS_NODEPTHTEST;
}

void CShaderPass::setFirstUnit()
{
    passflags |= PASS_FIRSTUNIT;
}

void CShaderPass::setLighting(bool use_lighting)
{
    if (use_lighting)
        passflags |= PASS_LIGHTING;
    else
        passflags &= ~PASS_LIGHTING;
}
