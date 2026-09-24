
/*
This file is part of DXShell
(c) 2002-2026 by Mathias Heyer

DXShell is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

DXShell is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "OGLErrors.h"

const char* OGLErrorToString(GLenum error)
{
    switch (error) {
    case GL_NO_ERROR:
        return "GL_NO_ERROR: No error has been recorded.";
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending function "
               "is ignored, having no side effect other than to set the error flag.";
    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE: A numeric argument is out of range. The offending function is ignored, having no "
               "side effect other than to set the error flag.";
    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending "
               "function is ignored, having no side effect other than to set the error flag.";
    case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW: This function would cause a stack overflow. The offending function is ignored, "
               "having no side effect other than to set the error flag.";
    case GL_STACK_UNDERFLOW:
        return "GL_STACK_UNDERFLOW: This function would cause a stack underflow. The offending function is ignored, "
               "having no side effect other than to set the error flag.";
    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the function. The state of OpenGL is "
               "undefined, except for the state of the error flags, after this error is recorded.";
    default:
        return "Unknown GL-Error";
    }
    return NULL;  // unsicher, ist aber eh nur hier um den compiler zu beruhigen
}
