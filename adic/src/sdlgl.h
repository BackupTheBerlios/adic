/*
 * Copyright (C) 2002 Jens Thiele <jens.thiele@student.uni-tuebingen.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*!
   \file sdlgl.h
   \brief class SDLGL
   \author Jens Thiele
*/

#ifndef SDLGL_H
#define SDLGL_H

#define DLOPEN_OPENGL

#if defined(__WIN32__) || defined(WIN32)
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#undef DLOPEN_OPENGL
#endif

#ifdef DLOPEN_OPENGL

#define FUNC(ret,name,parm) extern ret (* name ) parm
#include "minigl.h"
#include "glfunctions.h"
#undef FUNC

#else
#include <GL/gl.h>
#endif

#include <dope/dope.h>

#define DEBUG_GL(msg)
//#define DEBUG_GL(msg) DOPE_MSG("DEBUG: OpenGL: ", msg)

#define GL_ERRORS() do{if (gl.printErrors()) DOPE_MSG("gl error at", "^^^^^");}while(0)

//! wrapper around OpenGL functions to allow dlopening OpenGL
/*!
  this class uses SDL to look up the symbols
  \example take a look at sdlglgui.cpp on how to use it
  \todo when cross-compiling with mingw we can't dlopen the native opengl libs
  i assume the opengl library included in the mingw dist is just a wrapper around
  the native opengl library - but I did not yet look at it
  for now i disable dlopening opengl for win platforms
*/
struct SDLGL
{
public:
  SDLGL();

  //! lookup symbols
  /*!
    \note SDL must be initialized
  */
  void init();
  
  //! print out current opengl errors
  /*!
    \return the number of errors
  */
  int printErrors();
};

#endif
