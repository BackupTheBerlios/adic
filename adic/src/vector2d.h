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
   \file vector2d.h
   \brief yet another 2d-vector
   \author Jens Thiele
*/

#ifndef VECTOR2D_H
#define VECTOR2D_H

#include "typedefs.h"
#include <iostream>
#include <math.h>
#include <assert.h>

//! yet another simple 2D-Vector
/*! keep everything inline because it is simple and should be fast */
class V2D
{
public:
  V2D()
  {
    m_v[0]=m_v[1]=0;
  }

  V2D(R x, R y)
  {
    m_v[0]=x;
    m_v[1]=y;
  }

  // use default copy constructor - compiler creates it
  // V2D(const V2D &o)

  V2D &add(const V2D &o)
  {
    m_v[0]+=o.m_v[0];
    m_v[1]+=o.m_v[1];
    return *this;
  }

  V2D &sub(const V2D &o)
  {
    m_v[0]-=o.m_v[0];
    m_v[1]-=o.m_v[1];
    return *this;
  }

  V2D &mul(R s)
  {
    m_v[0]*=s;
    m_v[1]*=s;
    return *this;
  }

  V2D &div(R s)
  {
    mul(R(1)/s); // todo are 2 multiplies and 1 division really better than 2 div
    return *this;
  }
  
  V2D operator+(const V2D &o) const
  {
    V2D r(*this);
    return r.add(o);
  }

  V2D operator-(const V2D &o) const
  {
    V2D r(*this);
    return r.sub(o);
  }

  V2D &operator+=(const V2D &o)
  {
    return add(o);
  }

  V2D &operator-=(const V2D &o)
  {
    return sub(o);
  }

  V2D& operator*=(R s)
  {
    return mul(s);
  }
  
  V2D& operator/=(R s)
  {
    return div(s);
  }
  
  V2D operator*(R s) const
  {
    V2D r(*this);
    r.mul(s);
    return r;
  }

  V2D operator/(R s) const
  {
    V2D r(*this);
    r.div(s);
    return r;
  }

  R operator*(const V2D &o) const
  {
    return dot(o);
  }

  R dot(const V2D &o) const
  {
    return m_v[0]*o.m_v[0]+m_v[1]*o.m_v[1];
  }

  R cross(const V2D &o) const
  {
    return m_v[0]*o.m_v[1]+m_v[1]*o.m_v[0];
  }
  
  R length() const
  {
    return norm2();
  }

  R normMax() const
  {
    return (m_v[0]>=m_v[1]) ? m_v[0] : m_v[1];
  }

  //! the 1-norm
  R norm1() const
  {
    return m_v[0]+m_v[1];
  }
  //! the 2-norm (
  R norm2() const
  {
    return sqrt(norm2sqr());
  }

  R norm2sqr() const
  {
    return dot(*this);
  }

  //! normalize (with 2-norm)
  V2D& normalize()
  {
    div(norm2());
    return *this;
  }
  
  bool operator==(const V2D &o) const
  {
    return (m_v[0]==o.m_v[0])&&(m_v[1]==o.m_v[1]);
  }

  //! access our vector like an array - we pass back a reference to our member
  /*!
    this allows direct access to a protected member which doesn't follow
    OOP pardigms but is verry convenient in the case of this simple vector
  */
  R& operator[](int i)
  {
    assert((!i)||(i==1));
    return m_v[i];
  }

  R getX() const
  {
    return m_v[0];
  }
  
  R getY() const
  {
    return m_v[1];
  }

  //! rotate vector 90 degree clockwise
  /*!
    \returns the modified vector
  */
  V2D &rot90()
  {
    R t=m_v[0];
    m_v[0]=m_v[1];
    m_v[1]=-t;
    return *this;
  }
  
  //! rotate vector clockwise
  /*!
    \param rad radians to rotate

    \returns a new vector
  */
  V2D rot(R rad) const
  {
    R cosx=cos(rad)*m_v[0];
    R sinx=sin(rad)*m_v[0];
    R cosy=cos(rad)*m_v[1];
    R siny=sin(rad)*m_v[1];
    return V2D(cosx+siny,cosy-sinx);
  }
  
  std::ostream &write(std::ostream &ost) const
  {
    ost << m_v[0] << "/" << m_v[1];
    return ost;
  }

  std::istream &read(std::istream &ist)
  {
    ist >> m_v[0] >> m_v[1];
    return ist;
  }

  //! the vector
  /*! we keep it public because we allow direct access with the operator[] anyway */
  R m_v[2]; 
};
DOPE_CLASS(V2D);

template <typename Layer2>
inline void composite(Layer2 &layer2, V2D &v)
{
  layer2.simple(v[0],"x").simple(v[1],"y");
}

inline std::ostream &operator<<(std::ostream &ost, const V2D &v)
{
  return v.write(ost);
}

inline std::istream &operator>>(std::istream &ist, V2D &v)
{
  return v.read(ist);
}

#endif
