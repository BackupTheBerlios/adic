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
   \file version.h
   \brief A Version number consting of major minor and micro
   \author Jens Thiele
*/

#ifndef VERSION_H
#define VERSION_H

#include <dope/utils.h>
#include <dope/typenames.h>

//! A Version number consting of major minor and micro
struct Version
{
  Version() : m_major(0), m_minor(0), m_micro(0)
  {}
  Version(int majv, int minv, int micv) : m_major(majv), m_minor(minv), m_micro(micv)
  {}
  
  int m_major;
  int m_minor;
  int m_micro;

  std::string asString() const
  {
    return anyToString(m_major)+"."+anyToString(m_minor)+"."+anyToString(m_micro);
  }
  
  template <typename Layer2>
  void composite(Layer2 &l2)
  {
    l2.SIMPLE(m_major).SIMPLE(m_minor).SIMPLE(m_micro);
  }
};

DOPE_CLASS(Version);
template <typename Layer2>
inline void composite(Layer2 &layer2, Version &v)
{
  v.composite(layer2);
}

#endif
