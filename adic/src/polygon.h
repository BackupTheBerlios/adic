/*
 * Copyright (C) 2002 
 * Jens Thiele <jens.thiele@student.uni-tuebingen.de>
 * Jens Schwarz
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
   \file polygon.h
   \brief Polygon
   \author Jens Thiele, Jens Schwarz
*/

#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include "vector2d.h"

//! class representing a polygon
class Polygon
{
protected:
  //! the polygon
  std::vector<V2D> m_lineloop;
public:
  Polygon(const std::vector<V2D> &lineloop) : m_lineloop(lineloop)
  {}

  //! is the point inside of the polygon ?
  /*!
    \param p the point

    \returns true if point is inside
  */
  bool inside(const V2D &p) const;
};


#endif
