/*
 * Copyright (C) 2002 Jens Thiele <karme@berlios.de>
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
   \file sdlmouse.h
   \brief class SDLMouse
   \author Jens Thiele
*/

#ifndef SDLMOUSE_H
#define SDLMOUSE_H

#include "sdlinputdev.h"

//! mouse implementation of generic SDLInputDev
class SDLMouse : public SDLInputDev
{
public:
  static SDLMouse* create(SDLSigFactory &sf, const InputDevName &n);

  typedef SDLKey Keys[4];
  
  static Keys keys[2];
  
  SDLMouse(SDLSigFactory &sf, const InputDevName &n);
  
  ~SDLMouse()
  {}

  void handleResize(SDL_ResizeEvent e);
  void handleMotion(SDL_MouseMotionEvent e);
  void handleButton(SDL_MouseButtonEvent e);
protected:
  int8_t scale(Uint16 v);
  void setY(int8_t y)
  {
    if (y==m_state.y) return;
    m_state.y=y;
    input.emit(m_state);
  }

  Uint16 mx,my;
};

#endif
