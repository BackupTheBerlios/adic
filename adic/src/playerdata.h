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
   \file playerdata.h
   \brief class PlayerData
   \author Jens Thiele
*/

#ifndef PLAYERDATA_H
#define PLAYERDATA_H

#include "typedefs.h"
#include <vector>
#include <string>

#include <dope/typenames.h>

//! fixed player data
class PlayerData
{
public:
  PlayerData();

  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(maxspeed).SIMPLE(minspeed)
      .SIMPLE(rotspeed).SIMPLE(acceleration).SIMPLE(hdamping).SIMPLE(vdamping)
      .SIMPLE(mass).SIMPLE(maxfitness).SIMPLE(minfitness).SIMPLE(endurance).SIMPLE(type)
      .SIMPLE(r).SIMPLE(recoverTime);
  }

  //! maximum speed
  R maxspeed;
  //! minimum speed
  R minspeed;
  //! rotation speed
  R rotspeed;
  //! acceleration
  R acceleration;
  //! damping of velocity not in our direction
  R hdamping;
  //! damping of velocity in our direction
  R vdamping;
  //! our mass
  R mass;
  //! maximum fitness
  R maxfitness;
  //! minimum fitness
  R minfitness;
  //! endurance / stamina (German: Ausdauer)
  R endurance;
  //! type (0=boy 1=girl 2=cowboy 3=gentleman 10=barrel 11=powerup)
  int8_t type;
  //! radius
  R r;
  //! how fast the player revocers
  R recoverTime;
protected:
};
DOPE_CLASS(PlayerData);
template <typename Layer2>
inline void composite(Layer2 &layer2, PlayerData &p)
{
  p.composite(layer2);
}

#endif
