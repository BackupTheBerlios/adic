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
  \file netstream.h
  \brief class NetStream
  \author Jens Thiele
*/

#ifndef NETSTREAM_H
#define NETSTREAM_H

#include <string>
#include <dope/network.h>
#include <dope/dopesignal.h>
#include "messages.h" // OutProto and InProto

struct NetStream
{
  NetStream(const std::string &name, unsigned short int port);

  bool select(TimeStamp *stamp)
  {
    return layer0.select(stamp);
  }

  void read()
  {
    si.read();
  }

  void readAll()
  {
    TimeStamp null;
    while (select(&null)) read();
  }
  
  InternetAddress adr;
  NetStreamBuf layer0;
  OutProto l2out;
  InProto l2in;
  SignalOutAdapter<OutProto> so;
  SignalInAdapter<InProto> si;
};

#endif
