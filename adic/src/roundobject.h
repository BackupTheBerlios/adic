/*
 * Copyright (C) 2002 Jens Thiele <karme@berlios.de>
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
   \file roundobject.h
   \brief Round object
   \author Jens Thiele
*/

#ifndef ROUNDOBJECT_H
#define ROUNDOBJECT_H

#include "circle.h"
#include "gameobject.h"

//! round object
/*!
  base class for stuff lying/moving around in the world
*/
struct RoundObject : public GameObject, public Circle
{
  RoundObject()
  {}
  
  RoundObject(const V2D &pos,R r) : Circle(pos,r)
  {}
  
  void rollback()
  {
    DOPE_ASSERT(*(static_cast<Circle *>(this))!=m_oldCircle);
    *(static_cast<Circle *>(this))=m_oldCircle;
    DOPE_ASSERT(*(static_cast<Circle *>(this))==m_oldCircle);
  }
  
  void commit()
  {
    setOldValues(*this);
  }

  void setOldValues(RoundObject &o)
  {
    m_oldCircle=*(static_cast<Circle *>(&o));
  }


  bool moved() const
  {
    return *(static_cast<const Circle *>(this))!=m_oldCircle;
  }
  
  bool collide(const Circle &c, V2D &cv)
  {
    if (!Circle::collide(c))
      return false;
    cv=m_pos-c.m_pos;
    return true;
  }

protected:
  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    Circle::composite(layer2);
    //    layer2.SIMPLE(m_oldCircle);
  }

  Circle m_oldCircle;
};

#endif
