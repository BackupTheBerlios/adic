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

#include <GL/gl.h>

//! wrapper around OpenGL functions to allow dlopening OpenGL
/*!
  this class uses SDL to look up the symbols
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
  
  typedef void (*voidFunc)(void);
  typedef void (*intFunc)(int);
  typedef void (*uintFunc)(unsigned int);
  typedef void (*uint2Func)(unsigned int,unsigned int);
  typedef void (*uintuintPFunc)(unsigned int, unsigned int *);
  typedef void (*uint2intFunc)(unsigned int,unsigned int,int);
  typedef void (*floatFunc)(float);
  typedef void (*fvec2Func)(float,float);
  typedef void (*fvec3Func)(float,float,float);
  typedef void (*fvec4Func)(float,float,float,float);
  typedef void (*fvec5Func)(float,float,float,float,float);
  typedef void (*fvec6Func)(float,float,float,float,float,float);
  typedef void (*dvec6Func)(double,double,double,double,double,double);
  typedef void (*ivec2Func)(int,int);
  typedef void (*ivec4Func)(int,int,int,int);
  typedef void (*uintfloatPFunc)(unsigned int, float *);


  typedef void (*glTexImage2DFunc) ( GLenum target, GLint level,
				     GLint internalFormat,
				     GLsizei width, GLsizei height,
				     GLint border, GLenum format, GLenum type,
				     const GLvoid *pixels );

#define LOOKUP(m,t) t m
  LOOKUP(Clear,uintFunc);
  LOOKUP(MatrixMode,uintFunc);
  LOOKUP(LoadIdentity,voidFunc);
  LOOKUP(Color3f,fvec3Func);
  LOOKUP(Color4f,fvec4Func);
  LOOKUP(Translatef,fvec3Func);
  LOOKUP(Scalef,fvec3Func);
  LOOKUP(Begin,uintFunc);
  LOOKUP(Vertex2i,ivec2Func);
  LOOKUP(Vertex2f,fvec2Func);
  LOOKUP(End,voidFunc);
  LOOKUP(Viewport,ivec4Func);
  LOOKUP(Ortho,dvec6Func);
  LOOKUP(ClearColor,fvec4Func);
  LOOKUP(PushMatrix,voidFunc);
  LOOKUP(PopMatrix,voidFunc);
  LOOKUP(GetFloatv,uintfloatPFunc);
  LOOKUP(LineWidth,floatFunc);
  LOOKUP(Flush,voidFunc);
  LOOKUP(Finish,voidFunc);
  LOOKUP(Enable,uintFunc);
  LOOKUP(Disable,uintFunc);
  LOOKUP(BlendFunc,uint2Func);
  LOOKUP(TexCoord2f,fvec2Func);
  LOOKUP(BindTexture,uint2Func);
  LOOKUP(GenTextures,uintuintPFunc);
  LOOKUP(TexParameteri,uint2intFunc);
  LOOKUP(TexImage2D,glTexImage2DFunc);
  LOOKUP(Rotatef,fvec4Func);
#undef LOOKUP
};

#endif
