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
   \file sdlinputdev.h
   \brief class SDLInputDev
   \author Jens Thiele
*/

#ifndef SDLINPUTDEV_H
#define SDLINPUTDEV_H

#include "input.h"
#include "sdlsigfactory.h"

//! SDL input device abstraction
class SDLInputDev : public SigC::Object
{
public:
  //! create input device
  static SDLInputDev* create(SDLSigFactory &sf, const InputDevName &n);

  SDLInputDev(const InputDevName &n) : m_devName(n)
  {
    m_state.devno=n.devno;
  }
  virtual ~SDLInputDev(){}

  SigC::Signal1<void, Input &> input;

  const InputDevName &getDevName() const
  {
    return m_devName;
  }
protected:
  Input m_state;
  InputDevName m_devName;
};

#endif
