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
   \file test-vector2d.cpp
   \brief Test the vector class
   \author Jens Thiele
*/

#include "vector2d.h"
#include <assert.h>

int main()
{
  V2D a(1,5);
  V2D b(6,3);
  V2D c(a+b);
  std::cout << a << "+" << b << "=" << c << std::endl;
  std::cout << c << "-" << b << "=" << c-b << "==" << a << std::endl;
  assert(c-b==a);
  std::cout << a << "*" << b << "=" << a*b << std::endl;
  std::cout << a << "*" << 5 << "=" << a*5 << std::endl;
  std::cout << a << "+=" << b << "=";
  // must be called seperately (otherwise a+=b is called first)
  std::cout << (a+=b) << std::endl;  
  a[0]=2;
  a[1]=0;
  b=a;
  b.rot90();
  std::cout << "a=b=="<<a<<" b.rot90()="<<b<<std::endl;
  std::cout << "a * b " << a*b << std::endl;
  std::cout << "a X b " << a.cross(b) << std::endl;
  std::cout << "length(a): sqrt("<<a.norm2sqr()<<")="<<a.length() << std::endl;
  return 0;
}
