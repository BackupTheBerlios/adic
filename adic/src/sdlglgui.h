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
   \file sdlglgui.h
   \brief gui implementation using SDL and OpenGL
   \author Jens Thiele
*/

#ifndef SDLGLGUI_H
#define SDLGLGUI_H

#include "gui.h"

class SDLGLGUI : public GUI
{
public:
  SDLGLGUI(Client &client, const GUIConfig &config) 
    : GUI(client,config) 
  {}
  bool init();
  bool step(R dt);
  ~SDLGLGUI(){killWindow();}
protected:

  void resize(int width, int height);
  void initSDL();
  void createWindow(const char* title, int width, int height, int bits, bool fullscreenflag);
  void initGL();
  void killWindow();

  void drawCircle(const V2D &p, float r);
  void drawWall(const Wall &wall);
  void drawPolygon(const std::vector<V2D> &p);
  
  Input i;
  int m_width;
  int m_height;

  typedef void (*voidFunc)(void);
  typedef void (*intFunc)(int);
  typedef void (*uintFunc)(unsigned int);
  typedef void (*floatFunc)(float);
  typedef void (*fvec2Func)(float,float);
  typedef void (*fvec3Func)(float,float,float);
  typedef void (*fvec4Func)(float,float,float,float);
  typedef void (*fvec5Func)(float,float,float,float,float);
  typedef void (*fvec6Func)(float,float,float,float,float,float);
  typedef void (*dvec6Func)(double,double,double,double,double,double);
  typedef void (*ivec4Func)(int,int,int,int);
  typedef void (*uintfloatPFunc)(unsigned int, float *);

#define LOOKUP(m,t) t m##P

  LOOKUP(glClear,uintFunc);
  LOOKUP(glMatrixMode,uintFunc);
  LOOKUP(glLoadIdentity,voidFunc);
  LOOKUP(glColor3f,fvec3Func);
  LOOKUP(glTranslatef,fvec3Func);
  LOOKUP(glBegin,uintFunc);
  LOOKUP(glVertex2f,fvec2Func);
  LOOKUP(glEnd,voidFunc);
  LOOKUP(glViewport,ivec4Func);
  LOOKUP(glOrtho,dvec6Func);
  LOOKUP(glClearColor,fvec4Func);
  LOOKUP(glPushMatrix,voidFunc);
  LOOKUP(glPopMatrix,voidFunc);
  LOOKUP(glGetFloatv,uintfloatPFunc);
  LOOKUP(glLineWidth,floatFunc);
  LOOKUP(glFlush,voidFunc);
  LOOKUP(glFinish,voidFunc);

#undef LOOKUP
};

#endif
