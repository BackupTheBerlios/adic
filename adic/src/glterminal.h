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
   \file glterminal.h
   \brief class GLTerminal
   \author Jens Thiele
*/

#ifndef GLTERMINAL_H
#define GLTERMINAL_H

#include "typedefs.h"
#include <sigc++/signal_system.h>
#include <list>
#include <string>

class GLFont;

//! a simple terminal class using opengl
class GLTerminal
{
public:
  //! create terminal
  /*!
    \param font the font to use
    \param _x left postion of terminal (pixels)
    \param _y top postion of terminal (pixels)
    \param _centered (center text horizontally?)
    \param _scrollspeed how fast the text is scrolled away
    \param _writespeed how fast the text is printed
  */    
  GLTerminal(const GLFont &font,
	     int _x, int _y,
	     bool _centered=false,
	     R _scrollspeed=4, R _writespeed=15);
  
  ~GLTerminal(){}

  //! step time forward and draw terminal
  void step(R dt);

  //! clear terminal
  void clear() 
  {
    rows.clear();
  }

  //! print text to terminal
  void print(const std::string &newText);

  //! signal emitted if a character is printed (you can use it for example to play a sound)
  SigC::Signal1<void, char> printed;

protected:
  typedef std::basic_string<char> String;

  const GLFont &m_font;

  int x,y;
  bool centered;
  std::list<String> rows;
  std::list<String> buffered;
  
  unsigned maxcols,maxrows;
  R stimer,wtimer;
  //! how long is a row visible in sec.
  R scrollspeed;
  //! how many characters/sec. ?
  R writespeed;
  
  void append(const String &t);
  void appendRow(String r);
};

#endif
