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
   \file player.h
   \brief A player in the game world
   \author Jens Thiele
*/

#ifndef PLAYER_H
#define PLAYER_H

#include "roundobject.h"

//! a player in the game world
class Player : public RoundObject
{
public:
  Player(const V2D &pos=V2D(0,0));
  
  bool step(R dt);

  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(m_speed).SIMPLE(m_direction).SIMPLE(m_ix).SIMPLE(m_iy);
  }

  void setControl(int8_t x, int8_t y)
  {
    m_ix=x;
    m_iy=y;
  }
  
protected:
  int8_t getX() const
  {
    return m_ix;
  }
  int8_t getY() const
  {
    return m_iy;
  }

  //! maximum speed
  static const R m_maxspeed;
  //! acceleration
  static const R m_acceleration;
  //! damping of velocity not in our direction
  static const R m_damping;

  //! our crrent speed vector
  V2D m_speed;
  //! our direction we are heading
  R m_direction;
  //! input x
  int8_t m_ix;
  //! input y
  int8_t m_iy;
};
DOPE_CLASS(Player);

template <typename Layer2>
inline void composite(Layer2 &layer2, Player &p)
{
  p.composite(layer2);
}

#endif
