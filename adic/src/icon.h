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
   \file icon.h
   \brief Icon
   \author Jens Thiele
*/

#ifndef ICON_H
#define ICON_H

#include "roundobject.h"

//! an icon
/*! f.e.:
  - key
  - power ups
  - ...
*/
class Icon : public RoundObject
{
public:
  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
  }
  bool step(R dt)
  {
    return true;
  }
};
DOPE_CLASS(Icon);
template <typename Layer2>
inline void composite(Layer2 &layer2, Icon &x)
{
  x.composite(layer2);
}

#endif
