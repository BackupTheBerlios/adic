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
   \file gameobject.h
   \brief A object in the game
   \author Jens Thiele
*/

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "circle.h"

//! a object in the game world
class GameObject
{
public:
  //! step time forward
  /*!
    \param dt time elapsed
    \returns true if object stays alive otherwise false
  */
  virtual bool step(R dt)=0;

  //! rollback last step
  virtual void rollback()=0;
  
  //! commit last step
  virtual void commit()=0;

  //! collide with a circle
  /*!
    \param c the circle to collide
    \param room the room to collide the circle with
    \param collision vector is only set if there is a collision

    \return true if there is a collision otherwise false
  */
  virtual bool collide(const Circle &c, V2D &cv)=0;
};

#endif
