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

  Input i;
};

#endif