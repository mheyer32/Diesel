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
// GLText.cpp: Implementierung der Klasse CGLText.
//
//////////////////////////////////////////////////////////////////////

#include "GLText.h"
#include "Shader.h"
#include <iostream> 
#include "renderer.h"
#include <win/appwindow.h>
#include <misc/exception.h>
#include <GL/glu.h>
//#include <memorytracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COLOR  CGLText::std_colorcodes[]=
{
	COLOR(255,0,0,255),
	COLOR(0,255,0,255),
	COLOR(255,0,255,255),
	COLOR(0,0,255,255),
	COLOR(0,255,255,255),
	COLOR(255,255,0,255),
	COLOR(255,255,255,255),
	COLOR(0,0,0,255),
	COLOR(255,255,255,255) // undefined color
};

using namespace std;

CGLText::CGLText()
{
	buffer=NULL;
	cursX=0;
	cursY=0;

	vbuffer=new CVertexBuffer;
	vbuffer->AddRef();
	vbuffer->mode=GL_QUADS;
	back_vbuffer=new CVertexBuffer;
	back_vbuffer->AddRef();
	back_vbuffer->mode=GL_QUADS;

	VECTOR2  texcoords[4]={VECTOR2(0,1),VECTOR2(1,1),VECTOR2(1,0),VECTOR2(0,0)};

	back_vbuffer->AllocArrays(MT_DYNAMIC,VERTEXARRAY,4);
	back_vbuffer->AllocArrays(MT_STATIC,TEXCOORDARRAY,4);
	back_vbuffer->setTexCoords(texcoords,4);
	back_vbuffer->num_vertices=4;

	renderer=Renderer::Instance();

	float il[4];

	renderer->getIdentityLighting(il);

	for (int c=0;c<9;++c)
	{
		colorcodes[c].r=(float)std_colorcodes[c].r*il[0];
		colorcodes[c].g=(float)std_colorcodes[c].g*il[1];
		colorcodes[c].b=(float)std_colorcodes[c].b*il[2];
		colorcodes[c].alpha=255;
	}

	textcolor=colorcodes[8];
	
	SetBufsize(40,30);
	SetFontWidth(16);

	CShader *font=CShader::findOrLoadShader("gfx/2d/bigchars");
	if (!font) throw CException("CGLText(): could  not find  shader \"gfx/2d/bigchars\"");

	SetFontShader(font);
	background=false;

	border.top=border.left=border.right=border.bottom=0;
	destrect.top=destrect.left=0;
	destrect.right=CAppWindow::Instance()->getWidth();
	destrect.bottom=CAppWindow::Instance()->getHeight();
}

CGLText::~CGLText()
{
	KILLARRAY(buffer);

	vbuffer->FreeArrays();
	back_vbuffer->FreeArrays();

	vbuffer->Release();
	back_vbuffer->Release();

	unregisterForMessage(CAppWindow::Instance(),(Msg::MESSAGEID)WM_SIZE);
}


void CGLText::BeginRender()
{

	//FIXME: make this cleaner, border-calculations are spreaded everywhere
	charwidth=(float)((destrect.right-border.right)-(destrect.left+border.left));
	charheight=(float)((destrect.bottom-border.bottom)-(destrect.top+border.top));

	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,CAppWindow::Instance()->getWidth(),CAppWindow::Instance()->getHeight(),0);
	glMatrixMode(GL_MODELVIEW);

	glDisable(GL_DEPTH_TEST);

	charwidth/=(float)cols;
	charheight/=(float)rows;

	num_vertices=0;
}

void CGLText::EndRender()
{

	vbuffer->num_vertices=num_vertices;

	FlushVertexArrays();

	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


void CGLText::Print(LPCSTR Text)
{
char *bufptr,*bufend,letter;
DWORD len,bufsize;

	len=strlen(Text);
	if (len>bufferlen)
	{
		cursX=cursY=0;
		memset(buffer,' ',bufferlen);
	}
	bufsize=bufferlen-cursY*cols-cursX;
	bufptr=buffer+bufferlen-bufsize;
	bufend=buffer+cols*rows;

	do
	{
		letter=*Text++;
		if (letter==0) break;
		if (letter=='\n')
		{
			cursY++;
			cursX=0;
			bufptr=buffer+cursY*cols;
		//	continue;
		}
		if (bufptr>=bufend)
		{
			memcpy(buffer,buffer+cols,bufferlen-cols);
			cursY=rows-1;
			cursX=0;
			bufptr=bufend-cols;
			memset(bufptr,' ',cols);
		}
		if(letter!='\n')
		{
			*(bufptr++)=letter;
			cursX++;
		}
		if (cursX>=cols)
		{
			cursX=0;
			cursY++;
		}
	} while(1);

}

void CGLText::PrintAt(LPCSTR Text,DWORD X,DWORD Y)
{DWORD tx,ty;

	tx=cursX;
	cursX=X;
	if (X>=cols) X=cols-1;
	ty=cursY;
	cursY=Y;
	if (Y>=rows) Y=rows-1;
	Print(Text);
	cursX=tx;
	cursY=ty;
}

void CGLText::RenderChars(LPCSTR Text,DWORD texlen,float X,float Y)
{
float tx,ty,nextx,nexty,nexttx,nextty;
unsigned char letter;
int	color=8;

	colorcodes[color]=textcolor;

	VECTOR3		*vertices=vbuffer->vertices[0];
	VECTOR2	*texcoords=vbuffer->texcoords;
	COLOR		*colors=vbuffer->colors;


	nexty=Y+charheight;
	for (DWORD i=0;i<texlen;i++)
	{
		letter=*Text++;
		if (letter=='^')
		{
			color=*Text++;
			color=(color-'1')%9;
			i++;
			continue;
		}
		nextx=X+charwidth;
		if (letter!=' ')
		{
			ty=(float)(letter>>4)*TXCW; // (0.0) links unten in der Textur ????
			nextty=ty+TXCW;
			tx=(float)(letter&0x0F)*TXCW;
			nexttx=tx+TXCW;
	
			vertices[num_vertices+1].x=vertices[num_vertices].x=X;
			vertices[num_vertices+2].y=vertices[num_vertices+1].y=nexty;
			vertices[num_vertices+3].x=vertices[num_vertices+2].x=nextx;
			vertices[num_vertices].y=vertices[num_vertices+3].y=Y;
			vertices[num_vertices].z=vertices[num_vertices+1].z=vertices[num_vertices+2].z=vertices[num_vertices+3].z=1.0f;

			texcoords[num_vertices+1].x=texcoords[num_vertices].x=tx;
			texcoords[num_vertices+2].y=texcoords[num_vertices+1].y=nextty;
			texcoords[num_vertices+3].x=texcoords[num_vertices+2].x=nexttx;
			texcoords[num_vertices].y=texcoords[num_vertices+3].y=ty;

			colors[num_vertices+3]=colors[num_vertices+2]=colors[num_vertices+1]=colors[num_vertices]=colorcodes[color];
			num_vertices+=4;
		}
		X=nextx;
	}
}

void CGLText::RenderBuffer()
{
char *ptr;
float y;
	BeginRender();
	RenderBackground();
	y=destrect.top+border.top;
	ptr=buffer;
	for (DWORD i=0;i<rows;i++)
	{
		RenderChars(ptr,cols,destrect.left+border.left,y);
		y+=charheight;
		ptr+=cols;
	}

	EndRender();
}
void CGLText::TextOut(float X,float Y,LPCSTR Text)
{
	BeginRender();
	RenderBackground();
	RenderChars(Text,strlen(Text),X,Y);
	EndRender();
}

void CGLText::SetFontShader(CShader *Font)
{

	if (Font==NULL)
	{
		throw CException("CGLText::SetFontShader(NULL) bad");
		return;
	}
	vbuffer->setShader(Font);
}

void CGLText::SetFontWidth(int Width)
{
	TXCW=1.0f/(float)Width;
}
void CGLText::SetBufsize(DWORD Columns,DWORD Rows)
{
	KILLARRAY(buffer)
	cols=Columns;
	rows=Rows;
	bufferlen=cols*rows;
	buffer=new char[bufferlen];
	memset(buffer,' ',bufferlen);
	
	int num_verts=bufferlen*4;

	vbuffer->FreeArrays();
	vbuffer->AllocArrays(MT_DYNAMIC,VERTEXARRAY|TEXCOORDARRAY|COLORARRAY,num_verts);
	vbuffer->mode=GL_QUADS;
}

void CGLText::FlushVertexArrays()
{
	if (vbuffer->num_vertices==0) return;

	RENDERINFO info;

	vbuffer->fillRenderInfo(info);

	renderer->Render(info);
}

void CGLText::SetColor(const COLOR &col)
{
	textcolor=col;
}

void CGLText::SetDestRect(const RECT &DestRect)
{
	destrect=DestRect;

}

void CGLText::SetBorder(const RECT &Border)
{
	border=Border;
}

void CGLText::AttachToWindow()
{
	registerForMessage(CAppWindow::Instance(),(Msg::MESSAGEID)WM_SIZE);
}

Msg::MSGRVAL CGLText::handleMessage(Msg::MessagingObject *sender, Msg::MESSAGEID msgId, const Msg::Param &parameters)
{
	destrect.right=CAppWindow::Instance()->getWidth();
	destrect.bottom=CAppWindow::Instance()->getHeight();
	return Msg::PASS_ON;
}

int CGLText::getNumRows() const
{
	return rows;
}

int CGLText::getNumColumns() const
{
	return cols;
}


void CGLText::SetBackgroundShader(CShader *Background)
{
	back_vbuffer->setShader(Background);

	background=(Background!=0);
}

void CGLText::RenderBackground()
{
	if (!background) return;
	
	VECTOR3 *verts=back_vbuffer->vertices[0];

	verts[0].x=verts[3].x=destrect.left;
	verts[1].x=verts[2].x=destrect.right;
	verts[0].y=verts[1].y=destrect.bottom;
	verts[2].y=verts[3].y=destrect.top;
	verts[3].z=verts[2].z=verts[1].z=verts[0].z=1.0f;

	RENDERINFO info;
	back_vbuffer->fillRenderInfo(info);
	renderer->Render(info);
}
