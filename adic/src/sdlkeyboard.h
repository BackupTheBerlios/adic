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
   \file sdlkeyboard.h
   \brief class SDLKeyboard
   \author Jens Thiele
*/

#ifndef SDLKEYBOARD_H
#define SDLKEYBOARD_H

#include "sdlinputdev.h"

//! keyboard implementation of generic SDLInputDev
class SDLKeyboard : public SDLInputDev
{
public:
  static SDLKeyboard* create(SDLSigFactory &sf, const InputDevName &n);

  typedef SDLKey Keys[4];
  
  static Keys keys[2];
  
  SDLKeyboard(SDLSigFactory &sf, const InputDevName &n);
  
  ~SDLKeyboard()
  {}

  bool handleKey(SDL_KeyboardEvent e);
protected:
  int minor;
};

#endif
