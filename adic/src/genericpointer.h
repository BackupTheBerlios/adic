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
   \file genericpointer.h
   \brief class GenericPointer
   \author Jens Thiele
*/

#ifndef GENERICPOINTER_H
#define GENERICPOINTER_H

#include <dope/streamtraits.h>
#include <dope/typenames.h>
#include <boost/smart_ptr.hpp>

//! generic pointer
/*!
  \todo 
  - how to document template params with doxygen
  - how to document template concepts with doxygen
  - take a look at boost concept checks
  - recheck that the default copy constructor is ok
  I think it only works if the factory acts as cache
*/
template <typename X, typename Address, typename Factory>
class GenericPointer
{
public:
  GenericPointer(){}
  GenericPointer(const Address &adr)
  {
    setAddress(adr);
  }
  
  ~GenericPointer(){}
  
  //! output stream - simply write out the address
  template <typename Layer2>
  inline void composite(Layer2 &layer2,True)
  {
    layer2.simple(m_address,"address");
  }
  //! input stream - read address and ask our factory
  template <typename Layer2>
  inline void composite(Layer2 &layer2,False)
  {
    Address a;
    layer2.simple(a,"address");
    setAddress(a);
  }

  X& operator*() const
  { 
    return *get(); 
  }
  X* operator->() const
  { 
    return get(); 
  }

  //! get referenced object
  X* get() const
  {
    return m_xp.get();
    /*
    if (!m_xp.get())
      fetch();
      return m_xp.get(); */
  }

  void setAddress(const Address &a) 
  {
    m_address=a;
    fetch();
  }
protected:
  void fetch()
  {
    Factory factory;
    factory.get(m_address,m_xp);
  }
  
  Address m_address;
  DOPE_SMARTPTR<X> m_xp;
};

//! \todo how many template arguments do we want to pickle ?
template <typename T1, typename T2, typename T3> 
struct TypeNameTrait<GenericPointer<T1,T2,T3> > 
{ 
  static DOPE_INLINE TypeNameType name() DOPE_FCONST 
  { 
#define LT '<'
#define GT '>'
#define SEP ','
#define PTR '*'
#define REF '&'
#define ARRAY "[]"

    return TypeNameType("GenericPointer")+LT +TypeNameTrait<T1>::name() 
                              +SEP+TypeNameTrait<T2>::name() 
                              +GT; 

#undef LT
#undef GT
#undef SEP
#undef PTR
#undef REF
#undef ARRAY

  } 
};

template <typename Layer2, typename X, typename Address, typename Factory>
inline void composite(Layer2 &layer2,  GenericPointer<X, Address, Factory> &p)
{
  p.composite(layer2,typename Layer2::Traits::OutStream());
}

#endif
