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
// ShaderLoader.h: Schnittstelle für die Klasse CShaderLoader.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADERLOADER_H__C411CF09_BDD5_42FB_A3B1_2736AF640CCB__INCLUDED_)
#define AFX_SHADERLOADER_H__C411CF09_BDD5_42FB_A3B1_2736AF640CCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <map>
#include <string>
#include <vector>

#include <adt/ConVar.h>
#include <file/Path.h>
#include <misc/Lexer.h>

#include "Shader.h"

class CShaderLoader
{
public:
    CShaderLoader();
    ~CShaderLoader();

    CShader* loadShader(const std::string& shadername);

    // should be moved into texturemanager
    void loadSkyTextures(const char* skybasename, CTexture** textures);

    static void clearShaderCache();
    static void cacheDirectory(const CPath& directory);
    static void cacheFile(const CPath& file);

protected:
    bool ParseShader(CShader* newshader);
    bool ParsePass(CShader* newshader);
    void ParseWaveForm(const char* func, const char* base, const char* amplitude, const char* phase, const char* freq,
                       CWave& wavefunc);

    GLenum ParseBlendFunc(const char* string);

    Lexer lex;

    struct CACHEENTRY
    {
        int position;
        int filenumber;
        CACHEENTRY()
        : position(-1)
        , filenumber(-1) {};
        CACHEENTRY(int p, int f)
        : position(p)
        , filenumber(f) {};
    };

    typedef std::map<std::string, CACHEENTRY> SCRIPTCACHE;
    typedef SCRIPTCACHE::iterator             SCRIPTITERATOR;

    static SCRIPTCACHE              s_scriptcache;  // caches shadername and appropriate shaderfilename
    static std::vector<std::string> s_scriptfiles;

    static ConVar r_compress_sky;  // use texture compression for skies
};

#endif  // !defined(AFX_SHADERLOADER_H__C411CF09_BDD5_42FB_A3B1_2736AF640CCB__INCLUDED_)
