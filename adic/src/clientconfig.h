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
   \file clientconfig.h
   \brief configuration data of clients
   \author Jens Thiele
*/

#ifndef CLIENTCONFIG_H
#define CLIENTCONFIG_H


#include "typedefs.h"
#include <stdlib.h>
#include "messages.h"
#include "guiconfig.h"
#include "soundconfig.h"
#include "commonconfig.h"

//! configuration all clients have in common (gui client and bot client)
struct ClientConfig : public CommonConfig
{
  ClientConfig() 
    : m_server("localhost"),
      m_lagCompensation(true)
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
  
  std::string m_server;
  UserSetting m_users;
  bool m_lagCompensation;
};
DOPE_CLASS(ClientConfig);
template <typename Layer2>
inline void composite(Layer2 &layer2, ClientConfig &c)
{
  composite(layer2,static_cast<CommonConfig &>(c));
  layer2.simple(c.m_server,"server")
    .simple(c.m_users,"users")
    .simple(c.m_lagCompensation,"lagCompensation");
}

//! configuration of GUI client
struct GUIClientConfig : public ClientConfig
{
  GUIClientConfig()
  {
  }

  GUIConfig m_gui;
  SoundConfig m_sc;
};
DOPE_CLASS(GUIClientConfig);
template <typename Layer2>
inline void composite(Layer2 &layer2, GUIClientConfig &c)
{
  composite(layer2,static_cast<ClientConfig &>(c));
  layer2.simple(c.m_gui,"gui").simple(c.m_sc,"sound");
}

//! configuration of bot client
struct BotClientConfig : public ClientConfig
{
  BotClientConfig() : implementation("KarmeBot")
  {}

  void setDefaults() 
  {
    if (m_users.users.empty()) {
      // by default create 4 bots in one team
      m_users.users.push_back(User("Rob",implementation.c_str()));
      m_users.users.push_back(User("Tinky",implementation.c_str()));
      m_users.users.push_back(User("Bob",implementation.c_str()));
      m_users.users.push_back(User("Lobster",implementation.c_str()));
    }
  }

  std::string implementation;
};
DOPE_CLASS(BotClientConfig);
template <typename Layer2>
inline void composite(Layer2 &layer2, BotClientConfig &c)
{
  composite(layer2,static_cast<ClientConfig &>(c));
  layer2.simple(c.implementation,"implementation");
}

#endif
