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
   \file sdljoystick.h
   \brief class SDLJoystick
   \author Jens Thiele
*/

#ifndef SDLJOYSTICK_H
#define SDLJOYSTICK_H

#include "sdlinputdev.h"

class SDLJoystick : public SDLInputDev
{
public:
  static SDLJoystick* create(SDLSigFactory &sf, const InputDevName &n);

  SDLJoystick(SDLSigFactory &sf, const InputDevName &n, SDL_Joystick *j);
  ~SDLJoystick()
  {
    SDL_JoystickClose(m_joy);
  }

  void handleEvent(SDL_JoyAxisEvent event);
protected:
  int8_t scale(Sint16 v);

  void setX(int8_t x) 
  {
    if (x==m_state.x) return;
    m_state.x=x;
    input.emit(m_state);
  }
  void setY(int8_t y)
  {
    if (y==m_state.y) return;
    m_state.y=y;
    input.emit(m_state);
  }

  SDL_Joystick *m_joy;
};

#endif
