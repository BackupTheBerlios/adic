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
#include <string>

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
  //! counts how many input fields are active
  /*!
    \note this is needed because we use SDL_EnableUNICODE
    \todo protect this field (threads)
  */
  static int usageCount;
public:

  //! create input field
  /*!
    \param def the default content - if not specified empty
  */
  SDLInputField(const std::string &def=std::string())
    : m_active(false), m_content(def)
  {
  }

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
  bool handleKey(SDL_KeyboardEvent e);

  //! activate or deactivate input field (focus)
  void setActive(bool active=true);

  bool isActive() const
  {
    return m_active;
  }
  
  //! get current content
  const std::string &getContent() const
  {
    return m_content;
  }
  //! set content
  void setContent(const std::string &c)
  {
    m_content=c;
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
