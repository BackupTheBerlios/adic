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
   \file animation.h
   \brief class Animation
   \author Jens Thiele
*/

#ifndef ANIMATION_H
#define ANIMATION_H

#include "typedefs.h"
#include "texture.h"
#include <boost/smart_ptr.hpp>
#include <string>
#include <vector>

class SDLGLGUI;

struct Animation
{
  Animation(SDLGLGUI &gui, const std::vector<std::string> &uris);
  Texture &getTexture() const;
  void step(R dt);
  
  std::vector<DOPE_SMARTPTR<Texture> > textures;
  R time;
};

#endif
