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
   \file clientconfig.h
   \brief class ClientConfig
   \author Jens Thiele
*/

#ifndef CLIENTCONFIG_H
#define CLIENTCONFIG_H


#include "typedefs.h"
#include <stdlib.h>
#include "messages.h"
#include "guiconfig.h"
#include "soundconfig.h"

struct ClientConfig
{
  ClientConfig() : m_port(ADIC_PORT), m_server("localhost")
  {}
  
  void setDefaults() 
  {
    if (m_users.users.empty()) {
      const char *tmp=getenv("USER");
      if (tmp)
	m_users.users.push_back(User(tmp));
      else
	m_users.users.push_back(User("Player 1"));
      /*
	tmp=getenv("HOSTNAME");
	if (tmp)
	m_users.team=tmp;
	else
	m_users.team="Unkown";
      */
    }
  }
  
  unsigned short int m_port;
  std::string m_server;
  GUIConfig m_gui;
  SoundConfig m_sc;
  UserSetting m_users;
};
DOPE_CLASS(ClientConfig);
template <typename Layer2>
inline void composite(Layer2 &layer2, ClientConfig &c)
{
  layer2.simple(c.m_port,"port").simple(c.m_server,"server")
    .simple(c.m_gui,"gui").simple(c.m_sc,"sound").simple(c.m_users,"users");
}

#endif
