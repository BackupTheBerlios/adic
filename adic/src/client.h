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
   \file client.cpp
   \brief client main
   \author Jens Thiele
*/

#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <exception>
// layer 2 argument vector parser
#include <dope/argvparser.h>
#include <dope/network.h>
#include <dope/dopesignal.h>

#include <signal.h>

#include "typedefs.h"
#include "game.h"
#include "messages.h"
#include "guiconfig.h"
#include "soundconfig.h"
#include "sound.h"
#include <stdlib.h>

struct ClientConfig
{
  ClientConfig() : m_port(40700), m_server("localhost")
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

class GUI;

class Client : public SigC::Object
{
protected:
  ClientConfig &m_config;
  Game m_game;
  bool m_quit;
  std::vector<PlayerID> m_playerIDs;
  DOPE_SMARTPTR<Sound> m_soundPtr;
  std::vector<std::string> m_songs;
  unsigned m_csong;
  DOPE_SMARTPTR<GUI> m_guiPtr;
  SignalOutAdapter<OutProto> *soPtr;
public:
  Client(ClientConfig &config);
  ~Client();

  void handleGreeting(DOPE_SMARTPTR<ServerGreeting> gPtr);
  void handleGame(DOPE_SMARTPTR<Game> gPtr);
  void handleCollision(V2D pos, R strength);
  void handlePlayerInput(DOPE_SMARTPTR<PlayerInput> iPtr);
  void handleNewClient(DOPE_SMARTPTR<NewClient> mPtr);
  void playNextSong();
  void printed(char c);
  void handleChatMessage(DOPE_SMARTPTR<ChatMessage> chatPtr);
  void handleEndGame(DOPE_SMARTPTR<EndGame> egPtr);
  
  int main();

  Game::WorldPtr getWorldPtr()
  {
    return m_game.getWorldPtr();
  }

  Game &getGame()
  {
    return m_game;
  }
  
  const Game::Players &getPlayers() const
  {
    return m_game.getPlayers();
  }
  const std::vector<PlayerID> &getMyIDs() const
  {
    return m_playerIDs;
  }

  std::string getPlayerName(PlayerID id) const;
protected:
  std::streambuf* m_cerrbuf;
  std::streambuf* m_coutbuf;
};

#endif
