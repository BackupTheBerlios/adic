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
   \file messages.h
   \brief Message classes
   \author Jens Thiele
*/

#include "typedefs.h"
#include "version.h"

class Greeting
{
public:
  Greeting() 
    : m_dopeVersion(dope_major_version,dope_minor_version,dope_micro_version),
      m_adicVersion(PACKAGE_MAJOR_VERSION,PACKAGE_MINOR_VERSION,PACKAGE_MICRO_VERSION)
  {}
  ~Greeting(){}
  
  Version m_dopeVersion;
  Version m_adicVersion;

  template <typename Layer2>
  void composite(Layer2 &l2)
  {
    l2.SIMPLE(m_dopeVersion).SIMPLE(m_adicVersion);
  }
protected:
};
DOPE_CLASS(Greeting);
template <typename Layer2>
inline void composite(Layer2 &layer2, Greeting &g)
{
  g.composite(layer2);
}
  
