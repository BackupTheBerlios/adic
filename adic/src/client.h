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
   \file client.cpp
   \brief client main
   \author Jens Thiele
*/

#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <exception>
#include <dope/network.h>
#include <dope/dopesignal.h>

#include <signal.h>

#include "game.h"
#include "sound.h"
#include "clientconfig.h"
#include "netstream.h"

class GUI;

//! GUI client
/*!
  \todo the BotClient class and this class do have a lot of code in common.
  Perhaps create a common base class
*/
class Client : public SigC::Object
{
protected:
  GUIClientConfig &m_config;
  Game m_game;
  bool m_quit;
  std::vector<PlayerID> m_playerIDs;
  DOPE_SMARTPTR<Sound> m_soundPtr;
  std::vector<std::string> m_songs;
  unsigned m_csong;
  DOPE_SMARTPTR<GUI> m_guiPtr;
public:
  Client(GUIClientConfig &config);
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
  void handlePing(DOPE_SMARTPTR<Ping> pingPtr);

  //! try to connect
  /*!
    \return true on success otherwise false
  */
  bool connect();
  
  void sendGreeting();
  void ping();
  
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
  GUIClientConfig &getConfig()
  {
    return m_config;
  }
protected:
  std::streambuf* m_cerrbuf;
  std::streambuf* m_coutbuf;

  DOPE_SMARTPTR<NetStream> m_streamPtr;

  typedef std::list<DOPE_SMARTPTR<PlayerInput > > InputQueue;
  //! stores input events if they happen in the future
  InputQueue m_inputQueue;
};

#endif
