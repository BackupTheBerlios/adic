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
   \file sdlinputfield.cpp
   \brief 
   \author Jens Thiele
*/

#include "sdlinputfield.h"
#include <iostream>

int SDLInputField::usageCount=0;

bool
SDLInputField::handleKey(SDL_KeyboardEvent e)
{
  if (!m_active)
    return false;
  bool pressed=e.state==SDL_PRESSED;
  SDLKey k(e.keysym.sym);

  if (!pressed) return true;

  // check for special keys
  switch (k) {
  case SDLK_RETURN:
  case SDLK_KP2:
  case SDLK_KP6:
  case SDLK_RIGHT:
  case SDLK_DOWN:
    {
      input.emit(m_content);
      printed.emit('\n');
      setActive(false);
      return true;
    }
    break;
  case SDLK_DELETE:
  case SDLK_BACKSPACE:
  case SDLK_LEFT:
    {
      unsigned s=m_content.size();
      if (s)
	m_content.resize(s-1);
      printed.emit(k);
      return true;
    }
    break;
  default:
    break;
  }

  Uint16 unicode(e.keysym.unicode);
  char ch;
  if ( (unicode & 0xFF80) == 0 ) {
    ch = unicode & 0x7F;
    if (ch>=32) {
      m_content+=ch;
      printed.emit(ch);
      return true;
    }
  }
  else {
    std::cerr << "An International Character.\n";
  }
  return false;
}

void
SDLInputField::setActive(bool active)
{
  if (m_active==active)
    return;
  if (active) {
    if (++usageCount==1)
      SDL_EnableUNICODE(true);
  }else{
    if (!(--usageCount))
      SDL_EnableUNICODE(false);
  }
  m_active=active;
}
