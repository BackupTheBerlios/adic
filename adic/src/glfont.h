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
   \file glfont.h
   \brief class GLFont
   \author Jens Thiele
*/

#ifndef GLFONT_H
#define GLFONT_H

#include "typedefs.h"
#include "texture.h"

class SDLGL;

class GLFont
{
public:
  GLFont(SDLGL &_gl, const DOPE_SMARTPTR<Texture> &_texPtr, int _tilex=16, int _tiley=16)
    : gl(_gl), texPtr(_texPtr), tilex(_tilex), tiley(_tiley)
  {}
  ~GLFont(){}

  void drawTextRow(const std::string &text, bool centered=false);
  void drawText(const std::string &text, bool centered=false);
  int getWidth() const 
  {
    return texPtr->getWidth()/tilex;
  }
  int getHeight() const
  {
    return texPtr->getHeight()/tiley;
  }
  void setColor(unsigned num, const float c[3]);
  unsigned numColors() const
  {
    return colors.size();
  }
  
  std::string getColor(unsigned num)
  {
    return std::string()+char(11+num);
  }
protected:
  SDLGL &gl;
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
