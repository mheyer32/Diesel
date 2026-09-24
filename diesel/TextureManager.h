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

#include <adt/Singleton.h>
#include <adt/ConCmd.h>
#include <list>
#include <string>

#include <file/Path.h>
#include "Texture.h"
#include "videotexture.h"

class CTextureManager : public Singleton<CTextureManager>
{
    DECLARE_SINGLETON(CTextureManager)

    friend class CTexture;

public:
    typedef std::list<CTexture*>  TEXTURELIST;
    typedef TEXTURELIST::iterator TEXTUREITERATOR;

    /** try to find a texture with the specified name among the registered ones */
    CTexture* findTexture(const std::string& name);

    /** either find or load the texture. if the texture needs to be loaded, the given options will be used */
    CTexture* findOrLoadTexture(const std::string& name, int options = 0);

    /** try to load the texture, use options (see CTexture: TEX_xxx defines) */
    CTexture* loadTexture(const std::string& filename, int options = 0);

    /** load given imagefile into given texture */
    bool loadTexture(CTexture* texture, const CPath& filename, int options = 0);

    /** either find or load the given Videotexture */
    CVideoTexture* findOrLoadVideoTexture(const std::string& name);

    /** try to load/upload all registered textures */
    void restoreAllTextures();

    const TEXTURELIST& getRegisteredTextures();

protected:
    void registerTexture(CTexture* texture);
    void unregisterTexture(CTexture* texture);

    static ConCmd cmd_texturelist;

    static void cb_texturelist(ConCmd& command, const std::string& args);

private:
    CTextureManager(void);
    virtual ~CTextureManager(void);

    TEXTURELIST registeredtextures;
};
