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
// Texture.cpp: Implementierung der Klasse CTexture.
//
//////////////////////////////////////////////////////////////////////

#include "Texture.h"

#include "texturemanager.h"

#include <defs.h>
#include <math/MathLib.h>  // for isPowerOfTwo()
#include <misc/Exception.h>
#include <opengl/OpenGL.h>

#include <gl/glu.h>

#include <cassert>
#include <iostream>

#include <MemoryTracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

int    CTexture::max_textureId = 0;
ConVar CTexture::r_picmip("r_picmip", "0");
ConVar CTexture::r_texturebits("r_texturebits", "0");
ConVar CTexture::r_ext_compressed_textures("r_ext_compressed_textures", "0");
ConVar CTexture::r_roundImagesDown("r_roundImagesDown", "1");
ConVar CTexture::r_textureMode("r_textureMode", "GL_LINEAR_MIPMAP_NEAREST", 0, CTexture::cmd_r_textureMode);
ConVar CTexture::r_texture_anisotropy("r_texture_anisotropy", "1.0", NOFLAGS, CTexture::cb_r_texture_anisotropy);

using namespace std;

CTexture::CTexture()
{
    CTextureManager::Instance()->registerTexture(this);

    Texture = 0;
    image   = NULL;

    texstate    = 0;
    loadoptions = 0;
    texturemode = getFilterForName((std::string)r_textureMode);
    textureId   = max_textureId++;

    Ambient[0]  = 1.0f;
    Ambient[1]  = 1.0f;
    Ambient[2]  = 1.0f;
    Ambient[3]  = 1.0f;
    Diffuse[0]  = 1.0f;
    Diffuse[1]  = 1.0f;
    Diffuse[2]  = 1.0f;
    Diffuse[3]  = 1.0f;
    Specular[0] = 0.1f;
    Specular[1] = 0.1f;
    Specular[2] = 0.1f;
    Specular[3] = 1.0f;
    Emissive[0] = 0.0f;
    Emissive[1] = 0.0f;
    Emissive[2] = 0.0f;
    Emissive[3] = 1.0f;
    Shininess   = 0.0f;
}

CTexture::~CTexture()
{

    freeTexture();

    CTextureManager::Instance()->unregisterTexture(this);

    cout << "Texture: " << m_name << " deleted" << endl;
}

// enforce lowercase names
void CTexture::setName(const std::string& newname)
{
    char temp[512];
    assert(newname.size() < 512);
    strcpy(temp, newname.c_str());
    _strlwr(temp);
    m_name = temp;
}

void CTexture::bindTexture()
{
    glBindTexture(GL_TEXTURE_2D, Texture);

    if (!(texstate & TEX_USEMATERIAL))
        return;

    glMaterialfv(GL_FRONT, GL_AMBIENT, Ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, Specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, Emissive);
    glMaterialf(GL_FRONT, GL_SHININESS, Shininess);
    glColor4fv(Diffuse);
}

bool CTexture::setImage(Image* newimage, int Options)
{
    assert(newimage);

    freeTexture();  // falls schonmal eine textur geladen wurde

    glGenTextures(1, &Texture);

    if (Texture == 0) {
        GLERROR("CTexture::setImage()");
        return false;
    }

    loadoptions = Options;

    glBindTexture(GL_TEXTURE_2D, Texture);

    setTexParameter(Options, TEX_CLAMP | TEX_NOMIPMAP);
    setTextureMode(texturemode);

    if (loadoptions & TEX_FLIPY) {
        flipY(newimage);  // FIXME: changes original image, rethink handling of newimage
    }

    int newwidth  = newimage->getWidth();
    int newheight = newimage->getHeight();
    width         = newwidth;
    height        = newheight;

    chooseSize(newwidth, newheight);

    if (newimage->getBytesPerPixel() & 1) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
    } else {
        glPixelStorei(GL_UNPACK_ALIGNMENT, newimage->getBytesPerPixel());
        glPixelStorei(GL_PACK_ALIGNMENT, newimage->getBytesPerPixel());
    }

    if ((width != newwidth) || (height != newheight)) {
        Image* tempimage = new Image(newwidth, newheight, newimage->getBytesPerPixel(),
                                     newimage->getDestinationFormat(), newimage->getSourceFormat());

        int rval = gluScaleImage(newimage->getSourceFormat(), newimage->getWidth(), newimage->getHeight(),
                                 GL_UNSIGNED_BYTE, newimage->getImageData(), tempimage->getWidth(),
                                 tempimage->getHeight(), GL_UNSIGNED_BYTE, tempimage->getImageData());

        if (rval) {
            std::cout << "gluScaleImage() failed, because:" << endl << gluErrorString(rval) << endl;
        }

        uploadMipmaps(tempimage, GL_TEXTURE_2D);

        delete tempimage;

        width  = newwidth;
        height = newheight;
    } else {
        uploadMipmaps(newimage, GL_TEXTURE_2D);
    }

    if (loadoptions & TEX_KEEPIMAGE) {
        image = newimage;
    }

    return true;
}

bool CTexture::uploadMipmaps(Image* srcimage, int texturetarget)
{
    assert(srcimage);

    int internalformat = srcimage->getDestinationFormat();

    internalformat = chooseInternalFormat(internalformat, !(loadoptions & TEX_DONTCOMPRESS));

    if (!(texstate & TEX_NOMIPMAP)) {
        if (COpenGL::Instance()->getCaps() & GLCAPS_GENERATE_MIPMAP) {
            setTexParameter(TEX_GENMIPMAP, TEX_GENMIPMAP);
        }
    }

    int newwidth  = srcimage->getWidth();
    int newheight = srcimage->getHeight();

    assert(isPowerOfTwo(newwidth));
    assert(isPowerOfTwo(newheight));

    glTexImage2D(texturetarget, 0, internalformat, newwidth, newheight, 0, srcimage->getSourceFormat(),
                 GL_UNSIGNED_BYTE, srcimage->getImageData());
    GLERROR("CTexture::uploadMipmaps() glTexImage2D");

    if (!(texstate & TEX_NOMIPMAP) && !(texstate & TEX_GENMIPMAP)) {
        if (newwidth > 1 || newheight > 1) {
            int   mipmap = 0;
            BYTE* temp   = new BYTE[newwidth * newheight * 4];

            do {
                if (newwidth > 1)
                    newwidth >>= 1;
                if (newheight > 1)
                    newheight >>= 1;

                mipmap++;

                int rval = gluScaleImage(srcimage->getSourceFormat(), srcimage->getWidth(), srcimage->getHeight(),
                                         GL_UNSIGNED_BYTE, srcimage->getImageData(), newwidth, newheight,
                                         GL_UNSIGNED_BYTE, temp);
                if (rval) {
                    std::cout << "gluScaleImage() failed, because:" << endl << gluErrorString(rval) << endl;
                }

                glTexImage2D(texturetarget, mipmap, internalformat, newwidth, newheight, 0, srcimage->getSourceFormat(),
                             GL_UNSIGNED_BYTE, temp);

                GLERROR("CTexture::uploadMipmaps()glTexImage2D");

            } while (newwidth > 1 || newheight > 1);
            KILLARRAY(temp);
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// setzt die Texture Parameter, es werden nur die Optionen geändert, deren Bits in Optmask
// auf 1 gesetzt enthalten sind
// erlaubt Ändern nur eines Paramteters ohne Kenntnis bereits gesetzter Parameter
////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTexture::setTexParameter(DWORD Options, DWORD OptMask)
{
    DWORD diff_texflags;

    diff_texflags = (texstate ^ Options) & OptMask;

    glBindTexture(GL_TEXTURE_2D, Texture);

    if (diff_texflags & TEX_CLAMP) {
        if (Options & TEX_CLAMP) {
            // FIXME: make ready for 3D Coordinates
            if (COpenGL::Instance()->isExtensionSupported("EXT_texture_edge_clamp")) {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            } else {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            }
            texstate |= TEX_CLAMP;
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            texstate &= ~TEX_CLAMP;
        }
    }
    if (diff_texflags & TEX_NOMIPMAP) {
        if (Options & TEX_NOMIPMAP) {
            texstate |= TEX_NOMIPMAP;
        } else {
            texstate &= ~TEX_NOMIPMAP;
        }
        setTextureMode(texturemode);
    }
    if (diff_texflags & TEX_GENMIPMAP) {
        if (Options & TEX_GENMIPMAP) {
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
            texstate |= TEX_GENMIPMAP;
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE);
            texstate &= ~TEX_GENMIPMAP;
        }
    }

    if (diff_texflags & TEX_USEMATERIAL) {
        texstate = (texstate & ~TEX_USEMATERIAL) | (Options & TEX_USEMATERIAL);
    }

    if (COpenGL::Instance()->isExtensionSupported("EXT_texture_filter_anisotropic")) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)r_texture_anisotropy);
    }
}

void CTexture::freeTexture()
{
    if (Texture != 0) {
        glDeleteTextures(1, &Texture);
        Texture = 0;
    }

    KILLOBJECT(image)
    width       = 0;
    height      = 0;
    texstate    = 0;
    loadoptions = 0;
}

void CTexture::flipY(Image* flipimage)
{
    assert(flipimage);
    int bytesperline = flipimage->getWidth() * flipimage->getBytesPerPixel();

    unsigned char* templine = new unsigned char[bytesperline];

    unsigned char *y1, *y2;
    y1 = flipimage->getImageData();
    y2 = flipimage->getImageData() + (flipimage->getHeight() - 1) * bytesperline;

    while (y2 > y1) {
        memcpy(templine, y1, bytesperline);
        memcpy(y1, y2, bytesperline);
        memcpy(y2, templine, bytesperline);
        y1 += bytesperline;
        y2 -= bytesperline;
    }

    KILLARRAY(templine)
}

bool CTexture::restoreTexture()
{
    if (!filename.empty()) {
        if (CTextureManager::Instance()->loadTexture(this, filename, loadoptions)) {
            return true;
        }
    }

    if (image)  // still have the pointer where the bitmap came from
    {
        return setImage(image, loadoptions);
    }

    return false;
}

std::string CTexture::getFilename() const
{
    return filename;
}

int CTexture::getFilterForName(std::string filtername)
{
    int filter = GL_LINEAR_MIPMAP_NEAREST;
    if (filtername == "GL_NEAREST")
        filter = GL_NEAREST;
    else if (filtername == "GL_LINEAR")
        filter = GL_LINEAR;
    else if (filtername == "GL_NEAREST_MIPMAP_NEAREST")
        filter = GL_NEAREST_MIPMAP_NEAREST;
    else if (filtername == "GL_NEAREST_MIPMAP_LINEAR")
        filter = GL_NEAREST_MIPMAP_LINEAR;
    else if (filtername == "GL_LINEAR_MIPMAP_NEAREST")
        filter = GL_LINEAR_MIPMAP_NEAREST;
    else if (filtername == "GL_LINEAR_MIPMAP_LINEAR")
        filter = GL_LINEAR_MIPMAP_LINEAR;

    return filter;
}

int CTexture::chooseInternalFormat(int format, int usecompression)
{
    COpenGL* ogl = COpenGL::Instance();

    if (format == GL_BGR_EXT)
        format = GL_RGB;
    else if (format == GL_BGRA_EXT)
        format = GL_RGBA;

    if (usecompression && (int)r_ext_compressed_textures &&
        (ogl->getCaps() & (GLCAPS_TEXTURE_COMPRESSION | GLCAPS_S3TC))) {
        // std::cout<<"using texture compression"<<std::endl;
        if ((int)r_ext_compressed_textures == 1) {
            switch (format) {
            case GL_RGBA:
            case 4:
                format = GL_COMPRESSED_RGBA_ARB;
                break;
            case GL_RGB:
            case 3:
                format = GL_COMPRESSED_RGB_ARB;
                break;
            default:
                goto NO_COMPRESSION;
            }
        } else {
            switch (format) {
            case GL_RGBA:
            case 4:
                format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                break;
            case GL_RGB:
            case 3:
                format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
                break;
            default:
                goto NO_COMPRESSION;
            }
        }
        return format;
    }

NO_COMPRESSION:

    int texbits = (int)r_texturebits;

    switch (format) {
    case GL_RGB:
    case 3:
        switch (texbits) {
        case 16:
            format = GL_RGB5;
            break;
        case 32:
            format = GL_RGB8;
            break;
        }
        break;
    case GL_RGBA:
    case 4:
        switch (texbits) {
        case 16:
            format = GL_RGBA4;
            break;
        case 32:
            format = GL_RGBA8;
            break;
        }
        break;
    }

    return format;
}

void CTexture::setTextureMode(int filter)
{
    glBindTexture(GL_TEXTURE_2D, Texture);

    if (texstate & TEX_NOMIPMAP) {
        if (filter == GL_NEAREST || filter == GL_NEAREST_MIPMAP_NEAREST || filter == GL_NEAREST_MIPMAP_LINEAR) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    }
    if (filter == GL_NEAREST || filter == GL_NEAREST_MIPMAP_NEAREST || filter == GL_NEAREST_MIPMAP_LINEAR) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    texturemode = filter;
}

bool CTexture::reserveTexture(int newwidth, int newheight, GLenum internalformat)
{
    freeTexture();

    glGenTextures(1, &Texture);

    if (Texture == 0) {
        return false;
    }

    width  = newwidth;
    height = newheight;

    glBindTexture(GL_TEXTURE_2D, Texture);
    setTexParameter(TEX_NOMIPMAP, TEX_NOMIPMAP);

    // FIXME: ist es wirklich erlaubt einen NULL-Pointer zu schicken?
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    GLERROR("CTexture::reserveTexture() glTexImage2D");
    return true;
}

void CTexture::chooseSize(int& imagewidth, int& imageheight)
{
    int lower, upper;

    if (!isPowerOfTwo(imagewidth, lower, upper)) {
        imagewidth = ((int)r_roundImagesDown) ? lower : upper;
    }
    if (!isPowerOfTwo(imageheight, lower, upper)) {
        imageheight = ((int)r_roundImagesDown) ? lower : upper;
    }

    int pm = (int)r_picmip;
    if (pm && !(loadoptions & TEX_NOPICMIP)) {
        while (pm > 0 && imagewidth > 1 && imageheight > 1) {
            imagewidth >>= 1;
            imageheight >>= 1;
            pm--;
        }
    }

    // FIXME: einmal am Anfang reicht
    int maxx, maxy;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxx);
    maxy = maxx;
    //	glGetTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_WIDTH,&maxx);
    //	glGetTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_HEIGHT,&maxy);

    if (imagewidth > maxx)
        imagewidth = maxx;
    if (imageheight > maxy)
        imageheight = maxy;
}

void CTexture::cb_r_texture_anisotropy(ConVar& var)
{
    if (!COpenGL::Instance()->isExtensionSupported("EXT_texture_filter_anisotropic")) {
        std::cout << "Extension not supported: 'EXT_texture_filter_anisotropic'" << std::endl;
        return;
    }
    float max_anisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy);
    if ((float)var < 1.0f) {
        std::cout << "minimum anisotropy: 1.0" << std::endl;
        var = 1.0f;
    } else if ((float)var > max_anisotropy) {
        var = max_anisotropy;
        std::cout << "maximum anisotropy supported: " << max_anisotropy << std::endl;
    }

    const CTextureManager::TEXTURELIST&          texlist = CTextureManager::Instance()->getRegisteredTextures();
    CTextureManager::TEXTURELIST::const_iterator tIt     = texlist.begin();

    for (; tIt != texlist.end(); ++tIt) {
        (*tIt)->bindTexture();
        // I won´t make this into a member-function, best kept here
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)var);
    }
}

void CTexture::cmd_r_textureMode(ConVar& var)
{
    int filter = getFilterForName((std::string)var);

    const CTextureManager::TEXTURELIST&          texlist = CTextureManager::Instance()->getRegisteredTextures();
    CTextureManager::TEXTURELIST::const_iterator tIt     = texlist.begin();

    for (; tIt != texlist.end(); ++tIt) {
        (*tIt)->setTextureMode(filter);
    }
}