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
   \file input.h
   \brief input message
   \author Jens Thiele
*/

#ifndef INPUT_H
#define INPUT_H

#include "game.h"

//! input control message
struct Input
{
  Input() : x(0), y(0), devno(0)
  {}
  
  int8_t x;
  int8_t y;
  int8_t devno;
  
  template <typename Layer2>
  void composite(Layer2 &l2)
  {
    l2.SIMPLE(x).SIMPLE(y).SIMPLE(devno);
  }
};
DOPE_CLASS(Input);
template <typename Layer2>
inline void composite(Layer2 &layer2, Input &g)
{
  g.composite(layer2);
}

struct PlayerInput
{
  PlayerInput(const Input &_i, Game::PlayerID _id) 
    : i(_i), id(_id)
  {}
  PlayerInput()
    : id(~0U)
  {}
  
  Input i;
  Game::PlayerID id;

  template <typename Layer2>
  void composite(Layer2 &l2)
  {
    l2.SIMPLE(i).SIMPLE(id);
  }
};
DOPE_CLASS(PlayerInput);
template <typename Layer2>
inline void composite(Layer2 &layer2, PlayerInput &g)
{
  g.composite(layer2);
}

#endif
