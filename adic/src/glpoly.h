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
  \file glpoly.h
  \brief class GLPoly
  \author Jens Thiele
*/

#ifndef GLPOLY_H
#define GLPOLY_H

#include "vector2d.h"
#include <vector>

//! class to draw a filled/textured polygon
/*
  currently implemented using glu functions
*/
class GLPoly
{
public:
  GLPoly(const std::vector<V2D> &_poly);

  void draw();
  
  ~GLPoly();
protected:
  unsigned l;
};

#endif
