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
// Texture.h: Schnittstelle für die Klasse CTexture.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURE_H__2F13BF60_D2A0_11D3_BD8A_0000E85E86C1__INCLUDED_)
#define AFX_TEXTURE_H__2F13BF60_D2A0_11D3_BD8A_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable: 4786)  // STL-überlängen-Meldungen unterdruecken
#pragma comment(lib, "glu32")

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL\gl.h>

#include <list>
#include <string>
#include <adt/RefCount.h>
#include <adt/smartpointer.h>
#include <adt/NamedObject.h>
#include <adt/ConVar.h>
#include <defs.h>

#include "image.h"

// Load Texture Optionen
#define TEX_CLAMP		(1<<0)		// initially set clamping mode
#define TEX_NOPICMIP	(1<<2)		// dont picmip
#define TEX_NOMIPMAP	(1<<1)		// dont create mipmaps
#define TEX_KEEPIMAGE  (1<<3)		// keep the image object, can be used for recreating the texture
									// memory will be freed when deleting the texture or calling FreeTexture()
#define TEX_FLIPY		(1<<4)		// flip bitmap in vertical direction before using it
#define TEX_DONTCOMPRESS	(1<<5)	// don´t use texture compression
#define TEX_USEMATERIAL	(1<<6)		// texture uses material
// these are internal	settings that cannot be used as option while loading a texture
#define TEX_GENMIPMAP	(1<<31)



class CTexture : public CRefCount, public CNamedObject
{

friend class C3DSLoader;

public:

typedef SmartPointer<CTexture>	SMARTPTR;
typedef std::list<SMARTPTR>		SMARTPTRLIST;
typedef SMARTPTRLIST::iterator	SMARTPTRITERATOR;

typedef std::list<CTexture*>	TEXTURELIST;
typedef TEXTURELIST::iterator	TEXTUREITERATOR;

	CTexture();

	/** bind the texture and (if set) proper material settings*/
	virtual void bindTexture();

	/** sets the given Bitmap as new Image
	note that this function will
	completely destroy the old texture and create a new one*/
	virtual bool setImage(Image* image,int Options);

	/** sets one ore more of the textures parameters
	Options and OptMask uses one ore more or´ed TEX_xx values
	Mask will contain set bits at the option that is to be changed
	so if you want to change texture repeating to "CLAMP", call 
	"setTexParameter(TEX_CLAMP,TEX_CLAMP)", to turn off clamping,
	"setTexParameter(0,TEX_CLAMP)" */

	void setTexParameter(DWORD Options,DWORD OptMask); // FIXME: durch allgemeinere SetOptions ersetzen

    /** free the texture */
	virtual void freeTexture();

	/** get the ID of the texture
	IDs are automatically assigned at construction. They are unique and not reused when deleteing the texture
	*/
	inline int getTextureId() const{return textureId;};

	/** if the texture was loaded from a file,the appropriate filename is returned
	*/
	std::string getFilename() const;
	void setFilename(const std::string &filename);

	/** self-explaining */
	int getWidth() const {return image->getWidth();};
	int getHeight() const {return image->getHeight();};

	/** set the filtermode for this texture, valid vlaues are all supported GL_xxxxxxxxx
	texture filters
	*/
	void setTextureMode(int filtermode);

	/** try to recreate the texture from the file it was loaded or from the system memory.
	a texture can only be recreated from memory, if loadTexture(),findOrLoadTexture() or setImage()
	where used with the TEXKEEPPOINTER loadoptions
	*/
	bool restoreTexture();

	/** create an ogl-textureobject with the given dimensions and internalformat
		no image data is uploaded
	*/
	bool reserveTexture(int newwidth, int newheight, GLenum internalformat);

	/** overload CNamedObject::setName() to enforce lowercase names*/
	virtual void setName(const std::string &newname);

	/** returnes the proper GL-enum for the given filtername
	*/
	static int getFilterForName(std::string filtername);

	/** chooses an internal format for the given format, based upon
	r_textureBits and r_ext_compressed_textures
	to forbid the use of a compressed internal format, usecompression should be zero
	*/
	static int chooseInternalFormat(int format,int usecompression=1);

	GLfloat Ambient [4];
	GLfloat Diffuse [4];
	GLfloat Specular [4];
	GLfloat Emissive [4];
	GLfloat Shininess;	

	static ConVar r_picmip;
	static ConVar r_texturebits;
	static ConVar r_ext_compressed_textures;
	static ConVar r_roundImagesDown;
	static ConVar r_textureMode;
	static ConVar r_texture_anisotropy;

protected:	

	virtual ~CTexture();

	/** upload all mipmapping levels to opengl
	should be only called if  uploadimage is already a valid power-of-two-sized
	image
	*/
	bool uploadMipmaps(Image *uploadimage,int texturetarget);

	/** chooses new height/width based on r_picmip, r_rounddown and loadoptions set
	*/
	void chooseSize(int &width,int &height);

	/** flips the given image vertically
	*/
	void flipY(Image *flipimage);

	Image *image;		// enthält breite,höhe,format, anz. der komponenten und zeiger auf Textur in Sysmem

	int height;
	int width;
	std::string	filename;	// FIXME: think about it

	GLuint	Texture;

	int		texstate;		// current texture parameters
	int		loadoptions;	//FIXME: Options und contain basically the same information!
	int		texturemode;	// current filtermode

	int		textureId;			// unique texture id
	static int max_textureId;

	static void cmd_r_textureMode(ConVar &var);
	static void cb_r_texture_anisotropy(ConVar &var);
};

#endif // !defined(AFX_TEXTURE_H__2F13BF60_D2A0_11D3_BD8A_0000E85E86C1__INCLUDED_)
