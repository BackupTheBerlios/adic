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
  \file commonconfig.h
  \brief class CommonConfig
  \author Jens Thiele
*/

#ifndef COMMONCONFIG_H
#define COMMONCONFIG_H

struct CommonConfig
{
public:
  CommonConfig() 
    : m_port(ADIC_PORT), m_metaServer("http://adic.berlios.de/metaserver/index.php")
  {}

  unsigned short int m_port;
  std::string m_dataPath;
  std::string m_metaServer;
};
DOPE_CLASS(CommonConfig);
template <typename Layer2>
inline void composite(Layer2 &layer2, CommonConfig &c)
{
  layer2.simple(c.m_port,"port")
    .simple(c.m_dataPath,"dataPath")
    .simple(c.m_metaServer,"metaServer");
}

#endif
