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
   \file door.h
   \brief A door
   \author Jens Thiele
*/

#ifndef DOOR_H
#define DOOR_H

#include "gameobject.h"
#include "world.h"

//! a door
class Door : public GameObject
{
  //! angle
  R angle;
  //! angle velocity
  R angleSpeed;
  //! maximum Angle
  R maxAngle;
  //! minimum Angle
  R minAngle;
  //! edge ID - \todo is not inititialized nor pickled
  FWEdge::EID eid;
  
  static const R damping;
public:
  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(angle).SIMPLE(angleSpeed).SIMPLE(maxAngle).SIMPLE(minAngle);
  }
  bool step(R dt)
  {
    angle+=angleSpeed*dt;
    if (angle>maxAngle) angle=maxAngle-(angle-maxAngle);
    if (angle<minAngle) angle=minAngle+(minAngle-angle);
    angleSpeed-=angleSpeed*damping*dt;
    return true;
  }
  bool collide(const Circle &c, V2D &cv);
  //! is this door closed ?
  /*!
    this is used to test if all doors of a room are closed

    \return true if door is closed
  */
  bool isClosed() const;
};
DOPE_CLASS(Door);
template <typename Layer2>
inline void composite(Layer2 &layer2, Door &x)
{
  x.composite(layer2);
}

#endif
