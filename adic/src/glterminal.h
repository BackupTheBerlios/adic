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
#include <sstream>
#include <sigc++/signal_system.h>
#include <list>

class SDLGLGUI;
class SDLGL;

class GLTerminal
{
public:
  //! create terminal
  /*!
    \param _gl the GL Functions
  */    
  GLTerminal(SDLGLGUI &_gui, int _x, int _y, bool _centered=false, R _scrollspeed=4, R _writespeed=15);
  
  ~GLTerminal(){}
  void step(R dt);

  void clear() 
  {
    out.str("");
    rows.clear();
  }

  std::ostringstream out;
  //! is emitted if a character is printed
  SigC::Signal1<void, char> printed;
  
protected:
  SDLGLGUI &gui;
  int x,y;
  bool centered;
  std::list<std::string> rows;
  std::list<std::string> buffered;
  
  unsigned maxcols,maxrows;
  R stimer,wtimer;
  //! how long is a row visible in sec.
  R scrollspeed;
  //! how many characters/sec. ?
  R writespeed;
  
  void append(const std::string &t);
  void appendRow(std::string r);
};

#endif
