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
   \file sdlinputfield.h
   \brief class SDLInputField
   \author Jens Thiele
*/

#ifndef SDLINPUTFIELD_H
#define SDLINPUTFIELD_H

#include "typedefs.h"
#include <SDL/SDL.h> // needed for the key symbols

//! input field modell
/*!
  this class represents an input field
  you must display the text yourself
  this is just the model not the view (model-view-controller pattern)

  \example how to draw a InputField (s.a. sdlglgui.cpp)
  gl.LoadIdentity();
  gl.Translatef(0,0,0);
  gl.Color3f(1.0,1.0,1.0);
  m_fontPtr->drawText(m_chatLine.getContent());
*/
class SDLInputField : public SigC::Object
{
public:
  SDLInputField()
    : m_active(false)
  {}

  ~SDLInputField(){}

  //! the content when enter was pressed
  SigC::Signal1<void, const std::string &> input;

  //! single characters
  /*! emitted when a key is pressed */
  SigC::Signal1<void, char> printed;

  //! input handling method
  /*!
    connect this to the SDLSigFactory
    \example sdlglgui.cpp:
    sf.keyEvent.connect(SigC::slot(m_chatLine,&SDLInputField::handleKey));

    \return true if key was handled otherwise false
  */
  bool handleKey(SDL_KeyboardEvent e)
  {
    if (!m_active)
      return false;
    bool pressed=e.state==SDL_PRESSED;
    SDLKey k(e.keysym.sym);

    if (!pressed) return true;

    // check for special keys
    switch (k) {
    case SDLK_RETURN:
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
      unsigned s=m_content.size();
      if (s)
	m_content.resize(s-1);
      printed.emit(k);
      return true;
    }

    Uint16 unicode(e.keysym.unicode);
    char ch;
    if ( (unicode & 0xFF80) == 0 ) {
      ch = unicode & 0x7F;
      if (ch>=32) {
	m_content+=ch;
	printed.emit(ch);
      }
    }
    else {
      std::cerr << "An International Character.\n";
    }
    return true;
  }

  //! activate or deactivate input field (focus)
  void setActive(bool active=true)
  {
    m_active=active;
    SDL_EnableUNICODE(active);
  }

  bool isActive() const
  {
    return m_active;
  }
  
  //! get current content
  const std::string &getContent() const
  {
    return m_content;
  }
  //! clear the input field
  void clear() 
  {
    m_content.clear();
  }
protected:
  //! is the input field active ?
  bool m_active;
  //! the content
  std::string m_content;
};

#endif
