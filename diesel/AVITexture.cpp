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
// AVITexture.cpp: Implementierung der Klasse CAVITexture.
//
//
// Most of this code is taken from a NeHe Tutorial
////////////////////////////////////////////////////////////////////

#include <opengl/OpenGL.h>
#include <iostream>
#include "AVITexture.h"
#include "Engine.h"  // for global Time

#include <MemoryTracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ConVar CAVITexture::r_maxVideoSize("r_maxVideoSize", "256");

using namespace std;

// taken from NeHe AVI-TExture tutorial
inline void flipIt(void* buffer, unsigned int size)
{
    unsigned char* b = (unsigned char*)buffer;
    for (unsigned int i = 0; i < size; ++i) {
        unsigned char t = b[0];
        b[0]            = b[2];
        b[2]            = t;
        b += 3;
    }
}

CAVITexture::CAVITexture()
{
    AVIFileInit();  // Opens The AVIFile Library

    long lastframe = 0;  // Last Frame Of The Stream
    int  aviwidth  = 0;  // Video Width
    int  aviheight = 0;

    playing = false;
    frame   = -1;

    hdc     = NULL;
    hBitmap = NULL;
    hdd     = NULL;
    pavi    = NULL;
    pgf     = NULL;
    pdata   = NULL;
}

CAVITexture::~CAVITexture()
{  // Release The Stream
    close();

    AVIFileExit();
}
bool CAVITexture::open(const std::string& file)  // Opens An AVI File (szFile)
{

    close();

    if (AVIStreamOpenFromFile(&pavi, file.c_str(), streamtypeVIDEO, 0, OF_READ, NULL) != 0) {
        std::cout << "Failed To Open The AVI Stream" << std::endl;
        return false;
    }

    AVIStreamInfo(pavi, &psi, sizeof(psi));  // Reads Information About The Stream Into psi

    aviwidth  = psi.rcFrame.right - psi.rcFrame.left;  // Width Is Right Side Of Frame Minus Left
    aviheight = psi.rcFrame.bottom - psi.rcFrame.top;  // Height Is Bottom Of Frame Minus Top

    num_frames = AVIStreamLength(pavi);  // The Last Frame Of The Stream
    fps        = 1000.0f * (float)num_frames /
          (float)AVIStreamSampleToTime(pavi, num_frames);  // Calculate Rough Milliseconds Per Frame

    width  = aviwidth;
    height = aviheight;

    chooseSize(width, height);
    if (width > (int)r_maxVideoSize)
        width = (int)r_maxVideoSize;
    if (height > (int)r_maxVideoSize)
        height = (int)r_maxVideoSize;

    bmih.biSize        = sizeof(BITMAPINFOHEADER);  // Size Of The BitmapInfoHeader
    bmih.biPlanes      = 1;                         // Bitplanes
    bmih.biBitCount    = 24;                        // Bits Format We Want (24 Bit, 3 Bytes)
    bmih.biWidth       = width;                     // Width We Want (256 Pixels)
    bmih.biHeight      = height;                    // Height We Want (256 Pixels)
    bmih.biCompression = BI_RGB;                    // Requested Mode = RGB

    if (hdc == NULL) {
        hdc = CreateCompatibleDC(0);  // Creates A Compatible Device Context
    }

    hBitmap = CreateDIBSection(hdc, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&pdata), NULL, NULL);
    SelectObject(hdc, hBitmap);  // Select hBitmap Into Our Device Context (hdc)

    pgf = AVIStreamGetFrameOpen(pavi, NULL);  // Create The PGETFRAME	Using Our Request Mode
    if (pgf == NULL) {
        std::cout << "Failed To Open The AVI Frame" << std::endl;
        return false;
    }

    hdd = DrawDibOpen();

    COpenGL* ogl = COpenGL::Instance();
    if (ogl->isExtensionSupported("GL_EXT_bgra")) {
        colorswap_needed = false;
    } else {
        colorswap_needed = true;
    }

    reserveTexture(width, height, GL_RGB);
    GLclampf p = 1.0f;
    glPrioritizeTextures(1, &Texture, &p);
    // Information For The Title Bar (Width / Height / Last Frame)
    std::cout << "NeHe's AVI Player: Width: " << aviwidth << " Height: " << aviheight << " Frames: " << num_frames
              << endl;
    return true;
}

void CAVITexture::grabAVIFrame(int frame)  // Grabs A Frame From The Stream
{
    if (!pgf) {
        std::cout << "CAVITexture::grabAVIFrame() avi stream not open" << std::endl;
        return;
    }
    LPBITMAPINFOHEADER lpbi;                                         // Holds The Bitmap Header Information
    lpbi       = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, frame);  // Grab Data From The AVI Stream
    char* data = (char*)lpbi + lpbi->biSize +
                 lpbi->biClrUsed * sizeof(RGBQUAD);  // Pointer To Data Returned By AVIStreamGetFrame

    // Convert Data To Requested Bitmap Format
    DrawDibDraw(hdd, hdc, 0, 0, width, height, lpbi, data, 0, 0, aviwidth, aviheight, 0);

    if (colorswap_needed) {
        // Swap The Red And Blue Bytes (GL Compatability)
        flipIt(pdata, width * height);
        // Update The Texture
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pdata);
    } else {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pdata);
    }
}

void CAVITexture::close(void)  // Properly Closes The Avi File
{
    if (hBitmap)
        DeleteObject(hBitmap);  // Delete The Device Dependant Bitmap Object
    if (hdc)
        DeleteDC(hdc);
    if (hdd)
        DrawDibClose(hdd);  // Closes The DrawDib Device Context
    if (pgf)
        AVIStreamGetFrameClose(pgf);  // Deallocates The GetFrame Resources
    if (pavi)
        AVIStreamRelease(pavi);  // Release The Stream

    hdc     = NULL;
    hBitmap = NULL;
    hdd     = NULL;
    pavi    = NULL;
    pgf     = NULL;
    pdata   = NULL;  // Release The File
}

void CAVITexture::bindTexture()
{
    glBindTexture(GL_TEXTURE_2D, Texture);
    if (playing) {
        float frametime = g_Time - starttime;
        int   newframe  = (int)(fps * frametime);
        newframe        = newframe % num_frames;
        if (newframe != frame) {
            frame = newframe;
            grabAVIFrame(frame);
        }
    }
}

void CAVITexture::play()
{
    starttime = g_Time;
    playing   = true;
}

void CAVITexture::stop()
{
    playing = false;
}
