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
   \brief all files using OpenGL should include this file and never the OpenGL headers
   \author Jens Thiele
*/

#ifndef SDLGL_H
#define SDLGL_H

#include "typedefs.h"

#if defined(WINDOOF)
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#endif

#ifdef DLOPEN_OPENGL

#define FUNC(ret,name,parm) extern ret (* name ) parm
#include "minigl.h"
#include "glfunctions.h"
#undef FUNC

#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <dope/dope.h>

#define DEBUG_GL(msg)
//#define DEBUG_GL(msg) DOPE_MSG("DEBUG: OpenGL: ", msg)

#define GL_ERRORS() do{if (printGLErrors()) DOPE_MSG("gl error at", "^^^^^");}while(0)

//! print OpenGL errors
/*
  \return the number of errors occured
*/
int
printGLErrors();


#ifdef DLOPEN_OPENGL
//! load OpenGL library at runtime
void
loadGL(const std::string &libGL);
//! load GLU library at runtime
void
loadGLU(const std::string &libGLU);
//! lookup GL and GLU symbols
void
lookupGLSymbols();
#endif
//! deinit GL and GLU symbols
/*
  sets all function pointers to zero
  for easier debugging
*/
void
deinitGLSymbols();

#endif
