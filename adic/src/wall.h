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
   \file wall.h
   \brief a wall in the game world
   \author Jens Thiele
*/

#ifndef WALL_H
#define WALL_H

#include "circle.h"
#include "line.h"

//! this is our Wall class
/*! 
  it is a wall with a pillar at each end
*/
class Wall
{
protected:
  Line m_l;
  
  //! pillar radius
  R m_pr;
  //! wall thickness (half of the real thickness)
  R m_wt;
  //! circle including everything
  Circle m_circle;
public:
  Wall(const Line &line=Line(), R pr=4, R wt=2.5) 
    : m_l(line), m_pr(pr), m_wt(wt), m_circle((m_l.m_a+m_l.m_b)/2,(m_l.m_b-m_l.m_a).length()/2+pr*3)
  {
  }
  

  const Line &getLine() const
  {
    return m_l;
  }
  R getPillarRadius() const
  {
    return m_pr;
  }
  R getWallWidth() const
  {
    return m_wt*2;
  }
  
  //! collide circle and wall
  /*
    \param cv collision vector - is only set if there is a collision and is the 
    "surface" normal

    \return true if there is a collision otherwise false
  */
  bool collide(const Circle &circle, V2D &cv) const;

  //! collide circle and wall
  /*
    \param cv collision vector - is only set if there is a collision and is the 
    "surface" normal
    \param dist the distance of the collision point to the start of the wall
                this is needed to calculate the angular momentum on collision
		(f.e. doors) - is only set if there is a collision

    \return true if there is a collision otherwise false
  */
  bool collide(const Circle &circle, V2D &cv, R &dist) const;
};

#endif
