/*
 * Copyright (C) 2002 Jens Schwarz
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
   \file test-line.cpp
   \brief test for the line class
   \author Jens Schwarz
*/

#include "line.h"
#include <assert.h>

int main()
{
  R r;
  V2D a(1,1);
  V2D b(4,4);
  V2D c(1,2);
  V2D d(4,5);
  V2D cp(0,0);
  Line l1(a, b);
  Line l2(b, c);
  if (l1.collide(l2,cp))
    std::cout << "Lines cross at: " << cp << std::endl;
  else
    std::cout << "Lines do not cross\n";
  return 0;
}
