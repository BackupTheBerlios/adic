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
  \file botclient.h
  \brief main for bot clients
  \author Jens Thiele
*/

#ifndef BOTCLIENT_H
#define BOTCLIENT_H



#include <iostream>
#include <exception>
// layer 2 argument vector parser
#include <dope/argvparser.h>
#include <dope/network.h>
#include <dope/dopesignal.h>

#include <signal.h>

#include "game.h"
#include "clientconfig.h"
#include "netstream.h"
#include "input.h"


class Bot;


//! bot client
class BotClient : public SigC::Object
{
protected:
  BotClientConfig &m_config;
  Game m_game;
  bool m_quit;
  std::vector<PlayerID> m_playerIDs;
public:
  BotClient(BotClientConfig &config);
  ~BotClient();

  void handleGreeting(DOPE_SMARTPTR<ServerGreeting> gPtr);
  void handleGame(DOPE_SMARTPTR<Game> gPtr);
  void handleCollision(V2D pos, R strength);
  void handlePlayerInput(DOPE_SMARTPTR<PlayerInput> iPtr);
  void handleNewClient(DOPE_SMARTPTR<NewClient> mPtr);
  void handleChatMessage(DOPE_SMARTPTR<ChatMessage> chatPtr);
  void handleEndGame(DOPE_SMARTPTR<EndGame> egPtr);
  //! try to connect
  /*!
    \return true on success otherwise false
  */
  bool connect();
  
  void sendGreeting();

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

  //! get configuration
  /*!
    \note although this field is protected we allow direct access
  */
  BotClientConfig &getConfig()
  {
    return m_config;
  }
protected:
  DOPE_SMARTPTR<NetStream> m_streamPtr;
  DOPE_SMARTPTR<Bot> m_botPtr;
};

#endif
