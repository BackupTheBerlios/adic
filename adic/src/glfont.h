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
   \file glfont.h
   \brief class GLFont
   \author Jens Thiele
*/

#ifndef GLFONT_H
#define GLFONT_H

#include "typedefs.h"
#include <dope/smartptr.h>
#include <vector>

class Texture;

//! a fixed width font representation/text printing class using opengl
/*!
  currently this implementation uses texture mapped quads.
  There is one texture representing the the font which is passed to
  the constructor.
  you can encode colors into the text to be printed
*/
class GLFont
{
public:
  //! construct font
  /*!
    \param _texPtr the texture containing the font
    \param _tilex tiles in x direction
    \param _tiley tiles in y direction
  */
  GLFont(const DOPE_SMARTPTR<Texture> &_texPtr, int _tilex=16, int _tiley=16);

  //! draw one row of text
  void drawTextRow(const std::string &text, bool centered=false) const;
  //! draw a text - possibly multiple rows
  void drawText(const std::string &text, bool centered=false) const;
  //! get width of one character
  int getWidth() const;
  //! get height of one character
  int getHeight() const;
  //! set color
  /*!
    \param num the color number
    \param c the color
  */
  void setColor(unsigned num, const float c[3]);
  //! get number of defined colors
  unsigned numColors() const
  {
    return colors.size();
  }
  //! get character encoding color
  /*!
    \param num the color number
  */
  std::string getColor(unsigned num)
  {
    return std::string()+char(11+num);
  }
protected:
  DOPE_SMARTPTR<Texture> texPtr;
  int tilex;
  int tiley;
  struct Color
  {
    float c[3];
  };
  
  std::vector<Color> colors;
};

#endif
