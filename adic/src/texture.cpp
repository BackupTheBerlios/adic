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
   \file texture.cpp
   \brief Texture
   \author Jens Thiele
*/

#include "typedefs.h"
#include "texture.h"
#include "sdlglgui.h"

Texture::Texture(SDLGLGUI &gl, const char* name)
{
  SDL_PixelFormat RGB_PixelFormat = {
    NULL,
    18,3,
    0,0,0,8,
    0,8,10,0,
    0xff,0xff00,0xff0000,0x0,
    0,
    0xff
  };
    
  SDL_PixelFormat RGBA_PixelFormat = {
    NULL,
    32,4,
    0,0,0,0,
    0,8,16,24,
    0xff,0xff00,0xff0000,0xff000000,
    0,
    0xff
  };

  SDL_Surface* image=loadImage(name);
  GLint internalformat;
  GLint format;
  GLenum type=GL_UNSIGNED_BYTE;
  SDL_PixelFormat* pixelformat;
  if (image->format->Amask) {
    // todo bug here !! alpha channel doesn't mean we have got rgba !! it could be a grayscale with alpha
    // We have got a alpha channel !
    internalformat=GL_RGBA8;
    format=GL_RGBA;
    haveAlphaChannel=true;
    pixelformat=&RGBA_PixelFormat;
    /*todo
      }else if (image->format->colorkey){
      // We have a transparent color
      // todo either simply create alpha channel or ?
    */
  }else{
    // as fallback convert to rgb
    internalformat=GL_RGB8;
    format=GL_RGB;
    haveAlphaChannel=false;
    pixelformat=&RGB_PixelFormat;
  }
  SDL_Surface* tmp=SDL_ConvertSurface(image,pixelformat,SDL_SWSURFACE);
  SDL_FreeSurface(image);
  image=tmp;
  tmp=NULL;
  width=image->w;
  height=image->h;
  gl.glGenTexturesP(1,&textureID);
  gl.glBindTextureP(GL_TEXTURE_2D,textureID);
  // scale linearly when image bigger than texture
  gl.glTexParameteriP(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
  // scale linearly when image smaller than texture
  gl.glTexParameteriP(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image,
  // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
  gl.glTexImage2DP(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format , type, image->pixels);
  SDL_FreeSurface(image);
}


SDL_Surface*
Texture::loadImage(const char* filename)
{
  SDL_Surface* s=IMG_Load(filename);
  if (!s) DOPE_FATAL("Could not load image");
  return s;
};
