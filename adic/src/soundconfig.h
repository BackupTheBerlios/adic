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
   \file soundconfig.h
   \brief sound configuration
   \author Jens Thiele
*/

#ifndef SOUNDCONFIG_H
#define SOUNDCONFIG_H

#include "typedefs.h"
#include <string>
#include <dope/typenames.h>

struct SoundConfig
{
  SoundConfig()
    : sdriver("sdlmixer"), srate(22050), sbits(16), sbuffers(512), stereo(true)
  {}
  
  std::string sdriver;
  std::string sdevice;
  int srate;
  int sbits;
  int sbuffers;
  bool stereo;

  template <typename Layer2>
  void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(sdriver).SIMPLE(sdevice).SIMPLE(srate).SIMPLE(sbits).SIMPLE(sbuffers).SIMPLE(stereo);
  }
};
DOPE_CLASS(SoundConfig);
template <typename Layer2>
inline void composite(Layer2 &layer2, SoundConfig &c)
{
  c.composite(layer2);
}

#endif
