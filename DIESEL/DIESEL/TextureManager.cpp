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
#include "texturemanager.h"

#include <iostream>

#include <file/filemanager.h>
#include "avitexture.h"
#include "bmploader.h"
#include "ftxloader.h"
#include "imageloader.h"
#include "jpgloader.h"
#include "roqtexture.h"
#include "texture.h"
#include "tgaloader.h"

#include <memorytracker.h>

ConCmd CTextureManager::cmd_texturelist("texturelist", cb_texturelist);

CTextureManager::CTextureManager(void)
{
}

CTextureManager::~CTextureManager(void)
{
}

void CTextureManager::registerTexture(CTexture* texture)
{
    registeredtextures.push_back(texture);
}

void CTextureManager::unregisterTexture(CTexture* texture)
{
    registeredtextures.remove(texture);
}

CTexture* CTextureManager::findTexture(const std::string& name)
{
    char temp[512];
    assert(name.size() < 512);
    strcpy(temp, name.c_str());

    std::string lowname = temp;

    TEXTUREITERATOR tIt = registeredtextures.begin();

    for (; tIt != registeredtextures.end(); ++tIt) {
        if (lowname == (*tIt)->getName())
            return *tIt;
    }

    return NULL;
}
CTexture* CTextureManager::findOrLoadTexture(const std::string& name, int options)
{
    CTexture* texture = findTexture(name);
    if (texture)
        return texture;

    return loadTexture(name, options);
}

CTexture* CTextureManager::loadTexture(const std::string& orgfilename, int options)
{
    CTexture* texture = new CTexture();

    texture->setName(orgfilename);

    if (loadTexture(texture, orgfilename, options)) {
        return texture;
    }

    texture->AddRef();
    texture->Release();

    return NULL;
}

bool CTextureManager::loadTexture(CTexture* texture, const CPath& orgfilename, int options)
{
    CPath filename = orgfilename;

    CFileManager* fman   = CFileManager::Instance();
    CFile*        file   = NULL;
    ImageLoader*  loader = NULL;

    if (!filename.hasExtension()) {
        std::cout << "CTextureManager::loadTexture() cannot determine type of imagefile: " << filename << std::endl;
        return false;
    }

    std::string ext = filename.getExtension();

    if (ext == "tga") {
        if (file = fman->open(filename)) {
            loader = new TGALoader();
            goto FILE_FOUND;
        }
        ext = "jpg";  // custom to q3 where .tga fils a referenced, but .jpgs are meant
        filename.setExtension(ext);
    }
    if (ext == "jpg") {
        if (file = fman->open(filename)) {
            loader = new JPGLoader();
            goto FILE_FOUND;
        }
        ext = "ftx";
        filename.setExtension(ext);
    }
    if (ext == "ftx") {
        if (file = fman->open(filename)) {
            loader = new FTXLoader();
            goto FILE_FOUND;
        }
    }
    if (ext == "bmp") {
        if (file = fman->open(filename)) {
            loader = new BMPLoader();
            goto FILE_FOUND;
        }
    }
    cout << "CTextureManager::loadTexture(): missing " << orgfilename << endl;
    return false;

FILE_FOUND:

    Image* image = loader->load(*file);

    file->close();
    delete file;

    if (!image) {
        std::cout << "CTextureManager::loadTexture(): something went wrong while loading image data" << std::endl;
        return NULL;
    }
    if (ext == "tga" || ext == "bmp") {
        options ^= TEX_FLIPY;
    }
    bool rval = texture->setImage(image, options);  // hope, this doesn´t go wrong;

    delete loader;

    if (!(options & TEX_KEEPIMAGE)) {
        delete image;
    }

    return rval;
}

CVideoTexture* CTextureManager::findOrLoadVideoTexture(const std::string& name)
{
    CTexture*      tex = findTexture(name);
    CVideoTexture* vidtex;
    if (vidtex = dynamic_cast<CVideoTexture*>(tex))
        return vidtex;  // FIXME: check for right type

    CPath videofile(name);

    if (!CFileManager::Instance()->findFile(videofile, videofile)) {
        // if file cannot be found directly, try video/....
        if (videofile.getType() == CPath::PT_RELATIVE) {
            videofile = CPath("video") / videofile;  // probably a .roq file
        }
        if (!CFileManager::Instance()->findFile(videofile, videofile)) {
            cout << "CShaderLoader::ParsePass() cannot find file:" << videofile << endl;
            return NULL;
        }
    }

    // FIXME: refactor ROQTexture and AVITexture into CVideoTexture
    if (videofile.getExtension() == "avi") {
        vidtex = new CAVITexture;
    } else if (videofile.getExtension() == "roq") {
        vidtex = new CROQTexture;
    } else {
        std::cout << "CTextureManager::findOrLoadVideoTexture(): unsupported videoformat:" << videofile.getExtension()
                  << endl;
        return NULL;
    }

    vidtex->setName(name);
    vidtex->open(videofile.getString());

    return vidtex;
}

void CTextureManager::restoreAllTextures()
{
    TEXTUREITERATOR tIt = registeredtextures.begin();
    for (; tIt != registeredtextures.end(); ++tIt) {
        if (!(*tIt)->restoreTexture()) {
            std::cout << "CTextureManager::restoreAllTextures(): could not restore texture: " << (*tIt)->getName()
                      << "Filename: " << (*tIt)->getFilename() << std::endl;
            // throw CException(string("CTexture::restoreAllTextures() could not restore texture:
            // ")+(*tIt)->getName()+string(" Filename: ")+(*tIt)->getFilename());
        }
    }
}

const CTextureManager::TEXTURELIST& CTextureManager::getRegisteredTextures()
{
    return registeredtextures;
}

void CTextureManager::cb_texturelist(ConCmd& command, const std::string& args)
{
    std::cout << "Texturename             RefCount" << endl;
    const CTextureManager::TEXTURELIST& registeredtextures = CTextureManager::Instance()->getRegisteredTextures();

    CTextureManager::TEXTURELIST::const_iterator tIt = registeredtextures.begin();
    for (; tIt != registeredtextures.end(); ++tIt) {
        std::cout << (*tIt)->getName() << " " << (*tIt)->getRefCount() << std::endl;
    }
    std::cout << "..." << registeredtextures.size() << endl;
}