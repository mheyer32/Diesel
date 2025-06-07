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
#pragma once

#include <math/vector3.h>
#include "Texture.h"
#include "Wave.h"
#include "glstate.h"

#define SHADER_MAX_NUM_ANIM_FRAMES 8
#define SHADER_MAX_NUM_TCMOD 6

/* Shaderpass flags */
#define PASS_GLTCGEN TEXUNIT_TCGEN
#define PASS_BLENDFUNC GLSTATE_BLENDING
#define PASS_ALPHAFUNC GLSTATE_ALPHATEST
#define PASS_DEPTHWRITE GLSTATE_DEPTHWRITE
#define PASS_LIGHTING GLSTATE_LIGHTING
#define PASS_DEPTHFUNC GLSTATE_DEPTHFUNC
#define PASS_NODEPTHTEST GLSTATE_NODEPTHTEST

#define PASS_GLSTATE_MASK                                                                                              \
    (PASS_BLENDFUNC | PASS_ALPHAFUNC | PASS_DEPTHWRITE | PASS_LIGHTING | PASS_DEPTHFUNC | PASS_NODEPTHTEST)
#define PASS_TEXUNIT_MASK (PASS_GLTCGEN)

#define PASS_LIGHTMAP (1 << 16)
#define PASS_ANIMMAP (1 << 19)
#define PASS_TEXENV                                                                                                    \
    ((1 << 22))                   // if PASS_TEXENV is set, it means that blending is needed
                                  // but can be expressed as texture environment
                                  // this way the renderer can decide on multitexturing very fast
#define PASS_FIRSTUNIT (1 << 23)  // this pass MUST be in the first texunit (due to limitations, such as Alphatesting)
#define PASS_DETAIL (1 << 25)

/* Gen functions */
enum RGBGENFUNC
{
    RGBGEN_NONE,
    RGBGEN_IDENTITY,
    RGBGEN_IDENTITYLIGHTING,
    RGBGEN_WAVE,
    RGBGEN_VERTEX,
    RGBGEN_EXACTVERTEX,  // mhhhm, was das nur sein soll
    RGBGEN_ONEMINUSVERTEX,
    RGBGEN_ENTITY,
    RGBGEN_ONEMINUSENTITY,
    RGBGEN_LIGHTINGDIFFUSE,
    RGBGEN_FLARE,
    RGBGEN_FOG,
    RGBGEN_CONST,
    RGBGEN_MATERIAL  // take diffuse colour (and alpha) from (texture)-material, don´t change it
};

/* Alpha Gen functions */
enum ALPHAGENFUNC
{
    ALPHAGEN_NONE,
    ALPHAGEN_IDENTITY,
    ALPHAGEN_IDENTITYLIGHTING,
    ALPHAGEN_WAVE,
    ALPHAGEN_VERTEX,
    ALPHAGEN_ONEMINUSVERTEX,
    ALPHAGEN_ENTITY,
    ALPHAGEN_ONEMINUSENTITY,
    ALPHAGEN_LIGHTINGDIFFUSE,
    ALPHAGEN_PORTAL,
    ALPHAGEN_LIGHTINGSPECULAR
};

/* tcmod functions */
enum TCMODFUNC
{
    TCMOD_NONE,
    TCMOD_SCALE,
    TCMOD_ROTATE,
    TCMOD_SCROLL,
    TCMOD_TRANSFORM,
    TCMOD_TURB,
    TCMOD_STRETCH,
};

// tcgen function
enum TCGENFUNC
{
    TCGEN_NONE,
    TCGEN_LIGHTMAP,
    TCGEN_ENVIRONMENT,
    TCGEN_VECTOR,
    TCGEN_FOG,
    TCGEN_SPHEREMAP
};

class CShaderPass
{

    friend class Renderer;

public:
    CShaderPass(void);
    ~CShaderPass(void);

    void addTexture(CTexture* texture);
    void setFPS(float fps);
    void setBlendFunc(int src, int dst);
    void setAlphaFunc(int func, float alpharef);
    void setDepthFunc(int depthfunc);
    void setRGBGen(RGBGENFUNC rgbgen);
    void setRGBGenWave(const CWave& rgbwave);
    void setRGBGenConst(float r, float g, float b);
    void setAlphaGen(ALPHAGENFUNC alphagen);
    void setAlphaGenWave(const CWave& alphawave);
    void setAlphaGenPortal(float portalrange);

    // FIXME: convert TCMOD functions into TCMod objects
    void addTCModScale(float s_scale, float t_scale);                                        // scale s and t
    void addTCModRotate(float speed);                                                        // in degrees per second
    void addTCModScroll(float s_speed, float t_speed);                                       // in textures per secons
    void addTCModTransform(float m00, float m01, float m10, float m11, float t0, float t1);  // see Q3 shadermanual
    void addTCModTurb(float base, float amplitude, float phase, float freq);
    void addTCModStretch(const CWave& stretch);

    void setTCGen(TCGENFUNC tcgen);
    void setTCGenVector(const VECTOR3& s, const VECTOR3& t);

    void setDetailPass();
    void setLightmapPass();
    void setDepthWrite(bool write);
    void setDepthTest(bool test);
    void setFirstUnit();
    void setLighting(bool use_lighting);

    int getTexEnv() const { return texenv; };
    int getUsedArrays() const { return usedarrays; };
    int getFlags() const { return passflags; };
    int getRGBGen() const { return rgbgen; };
    int getAlphaGen() const { return alphagen; };
    int getTCGen() const { return tcgen; };
    int getDepthFunc() const { return depthfunc; };

protected:
    int passflags;
    int usedarrays;  // arrays that this pass needs;

    GLenum blendsrc, blenddest;
    GLenum texenv;

    GLenum depthfunc;

    GLenum alphafunc;
    float  alphafuncref;

    RGBGENFUNC rgbgen;
    CWave      rgbgen_func;
    float      rgbgen_const[3];

    ALPHAGENFUNC alphagen;
    CWave        alphagen_func;
    float        alphagen_portalrange;

    // each mod should be only used once
    int       tcmod_num;
    TCMODFUNC tcmod_order[SHADER_MAX_NUM_TCMOD];
    float     tcmod_scale[2];
    float     tcmod_rotate;
    float     tcmod_scroll[2];
    float     tcmod_transform[6];
    float     tcmod_turb[4];
    CWave     tcmod_stretch;

    TCGENFUNC tcgen;
    VECTOR3   tcgen_vector[2];

    float              anim_fps; /* Animation frames per sec */
    int                anim_numframes;
    CTexture::SMARTPTR anim_frames[SHADER_MAX_NUM_ANIM_FRAMES]; /* Texture refs */
};
