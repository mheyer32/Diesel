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
// GLText.h: Schnittstelle für die Klasse CGLText.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLTEXT_H__5F472055_77E8_4CF1_A911_654DA21F52AB__INCLUDED_)
#define AFX_GLTEXT_H__5F472055_77E8_4CF1_A911_654DA21F52AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Messaging/MessagingObject.h>
#include "enginetypes.h"
#include "vertexbuffer.h"

class CShader;
class Renderer;


class CGLText : protected Msg::MessagingObject
{


public:
	CGLText();
	~CGLText();
	
	void SetBufsize(DWORD Columns, DWORD Rows);
	void RenderBuffer();
	void Print(LPCSTR Text);
	void PrintAt(LPCSTR Text,DWORD x, DWORD y);
    void TextOut(float x, float y,LPCSTR Text);

	void SetBackgroundShader(CShader *Font);
	void SetFontShader(CShader *Background);
	void SetFontWidth(int Width);
	void SetColor(const COLOR &col);

	void AttachToWindow();
	void SetDestRect(const RECT &DestRect);
	void SetBorder(const RECT &Border);

	inline void Clr()
	{
		memset(buffer,' ',bufferlen);
		cursX=cursY=0;
	}
	inline void Locate(DWORD X,DWORD Y)
	{
		cursX=X;cursY=Y;
	}

	int getNumColumns() const;
	int getNumRows() const;

protected:

		virtual	 Msg::MSGRVAL handleMessage(Msg::MessagingObject *sender, Msg::MESSAGEID msgId, const Msg::Param &parameters=Msg::Param());


		void RenderChars(LPCSTR Text,DWORD texlen,float X,float Y);
		void RenderBackground();
		void BeginRender();
		void EndRender();
		void FlushVertexArrays();
		
		DWORD cursX,cursY,cols,rows,bufferlen;
		float charwidth,charheight;
		char *buffer;

		float TXCW;
		RECT destrect;
		RECT border;
		bool background;


		CVertexBuffer	*vbuffer;
		CVertexBuffer	*back_vbuffer;

		int	num_vertices;

		COLOR	textcolor;

		Renderer *renderer;
		
		COLOR  colorcodes[9];
		static COLOR  std_colorcodes[9];
};

#endif // !defined(AFX_GLTEXT_H__5F472055_77E8_4CF1_A911_654DA21F52AB__INCLUDED_)
