/*
 * Copyright (C) 2001-2002 Jens Thiele <karme@unforgettable.com>
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
 *  
 */

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "typedefs.h"

//! class to wrap time functions
/*
  thin wrapper class around gettimeofday or similar functions
*/
class TimeStamp
{
public:
  // default constructor - time is set to 0
  TimeStamp() : m_sec(0), m_usec(0) {}
  //! set time
  TimeStamp(int sec, int usec) : m_sec(sec), m_usec(usec) 
  {}
  
  //! set to current time
  void now();
  
  //! difference between 2 times
  /*!
    \attention The o time must be smaller then this time (*this>=o)
    \todo think about it
  */
  TimeStamp operator-(const TimeStamp &o) const;

  //! sleep
  void sleep() const;

  //! get seconds
  int getSec() const 
  {
    return m_sec;
  }
  //! get micro-seconds
  int getUSec() const
  {
    return m_usec;
  }

  bool operator<(const TimeStamp &o) const
  {
    if (m_sec==o.m_sec) return (m_usec<o.m_usec);
    else if (m_sec<o.m_sec) return true;
    return false;
  }
  
  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(m_sec).SIMPLE(m_usec);
  }
protected:
  //! seconds
  int m_sec;
  //! microseconds
  int m_usec;
};
DOPE_CLASS(TimeStamp);

template <typename Layer2>
inline void composite(Layer2 &layer2, TimeStamp &s)
{
  s.composite(layer2);
}


#endif
