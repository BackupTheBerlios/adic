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
   \file guiconfig.h
   \brief GUIConfig
   \author Jens Thiele
*/

#ifndef GUICONFIG_H
#define GUICONFIG_H

#include "typedefs.h"

struct GUIConfig
{
  // todo - default for libGL should be different on win
  GUIConfig() 
    : implementation("SDLGLGUI"), title(PACKAGE), libGL(""), 
    width(640), height(480), bits(32), fullscreen(false), quality(0), flush(true)
  {}
  
  std::string implementation;
  std::string title;
  std::string libGL;
  int width;
  int height;
  int bits;
  bool fullscreen;
  //! display quality (f.e. texture mapping)
  int quality;
  //! should we flush from time to time (default is yes because otherwise i had trouble with mesa)
  bool flush;
  
  template <typename Layer2>
  void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(implementation).SIMPLE(title).SIMPLE(libGL)
      .SIMPLE(width).SIMPLE(height).SIMPLE(bits).SIMPLE(fullscreen).SIMPLE(quality)
      .SIMPLE(flush);
  }
};
DOPE_CLASS(GUIConfig);
template <typename Layer2>
inline void composite(Layer2 &layer2, GUIConfig &c)
{
  c.composite(layer2);
}

#endif
