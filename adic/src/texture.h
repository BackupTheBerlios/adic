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
   \file texture.h<2>
   \brief texture class
   \author Jens Thiele
*/


#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

class SDLGLGUI;

class Texture
{
public:
  Texture(SDLGLGUI &gui, const char* name);

  bool isTransparent() const {return haveAlphaChannel;}
  int getWidth() const {return width;}
  int getHeight() const {return height;}
  GLuint getTextureID() const {return textureID;}
    
protected:
  GLuint textureID;
  int width;
  int height;
  bool haveAlphaChannel;


  static SDL_Surface* loadImage(const char* filename);
};

#endif
