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
   \file line.h
   \brief Line
   \author Jens Thiele, Jens Schwarz
*/

#ifndef LINE_H
#define LINE_H

#include "vector2d.h"

//! line (vector) from a to b
class Line
{
public:
  Line()
  {
    m_a = V2D(0,0);
    m_b = V2D(1,1);
    m_v = V2D(m_b-m_a);
    m_vn = V2D(m_v);
    m_vn.normalize();
    m_n = V2D(m_vn.rot90());
  }

  Line(const V2D &a, const V2D &b)
  {
    m_a=a;
    m_b=b;
    m_v = V2D(m_b-m_a);
    m_vn = V2D(m_v);
    m_vn.normalize();
    m_n = V2D(m_vn.rot90());
  }
    
  V2D m_a,m_b,m_v,m_vn,m_n;

  //! parallel projection of point p on Line orthogonal to line
  /*!
    find foot point (f)
    
    \returns parameter r in f=a+(b-a)*r; with (f-a)*(p-f)=0
  */
  R project(const V2D &p);

  //! distance of point to line
  R dist(const V2D &p);
  
  //! collide two line
  /*!
    \param cp collision point (only set if there is a collision - cross point)
    
    \returns true if the lines collide/cross otherwise false
  */
  bool collide(const Line &o, V2D &cp);

  //! same as above but does not return cp
  bool collide(const Line &o); 

  //! same as above but does return the collision as parameter r in cp=a+(b-a)*r
  bool collide(const Line &o, R &cp);

  //! translate line
  void translate(const V2D &tv)
  {
    m_a+=tv;
    m_b+=tv;
  }

  //! return "surface" normal
  V2D normal() const
  {
    return (m_b-m_a).rot90();
  }
};

#endif
