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

#define RAD(d) R(d)*M_PI/R(180)

//! a door
/*! abstract door not in the game world */
class Door 
// : public GameObject not a gameobject because you can't collide with it
{
  //! angle
  R angle;
  //! angle velocity
  R angleSpeed;
  //! maximum Angle
  R maxAngle;
  //! minimum Angle
  R minAngle;
  //! edge ID
  FWEdge::EID eid;
  

  R oldAngle;
  R oldAngleSpeed;

  bool locked;
  
  static const R damping;
public:
  Door(FWEdge::EID _eid=FWEdge::noEdge)
    : angle(0), angleSpeed(RAD(10)), maxAngle(RAD(90)), minAngle(RAD(-90)), eid(_eid), oldAngle(0), oldAngleSpeed(angleSpeed),
      locked(false)
  {}

  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(angle).SIMPLE(angleSpeed).SIMPLE(maxAngle).SIMPLE(minAngle).SIMPLE(eid);
  }
  bool step(R dt)
  {
    angle+=angleSpeed*dt;
    if (angle>maxAngle) {angle=maxAngle-(angle-maxAngle);angleSpeed*=-1;}
    if (angle<minAngle) {angle=minAngle+(minAngle-angle);angleSpeed*=-1;}
    angleSpeed-=angleSpeed*damping*dt;
    return true;
  }

  //! is this door closed ?
  /*!
    this is used to test if all doors of a room are closed

    \return true if door is closed
  */
  bool isClosed() const;

  bool isLocked() const
  {
    return locked;
  }

  FWEdge::EID getEdgeID() const
  {
    return eid;
  }
  
  void rollback()
  {
    angle=oldAngle;
    angleSpeed=oldAngleSpeed;
  }
  
  void commit()
  {
    oldAngle=angle;
    oldAngleSpeed=angleSpeed;
  }

  //! get angular speed
  R getSpeed() const
  {
    return angleSpeed;
  }
  //! get angle
  R getAngle() const
  {
    return angle;
  }
  void addSpeed(R s)
  {
    angleSpeed+=s;
  }
};
DOPE_CLASS(Door);
template <typename Layer2>
inline void composite(Layer2 &layer2, Door &x)
{
  x.composite(layer2);
}

#endif
