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
   \file circle.h
   \brief A circle
   \author Jens Thiele
*/

#ifndef CIRCLE_H
#define CIRCLE_H

#include "vector2d.h"

class Circle
{
public:
  Circle() : m_r(0)
  {}
  
  Circle(const V2D &pos, R radius) : m_pos(pos) , m_r(radius)
  {}
  
  bool collide(const Circle &o) const
  {
    R maxdist=(o.m_r+m_r);
    return (o.m_pos-m_pos).norm2sqr()<=maxdist*maxdist;
  }

  V2D m_pos;
  R m_r;

  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(m_pos).SIMPLE(m_r);
  }
};
DOPE_CLASS(Circle);

template <typename Layer2>
inline void composite(Layer2 &layer2, Circle &c)
{
  c.composite(layer2);
}

#endif
