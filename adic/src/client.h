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

#include "game.h"
#include "sound.h"
#include "clientconfig.h"

class GUI;

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
  //! try to connect
  /*!
    \return true on success otherwise false
  */
  bool connect();
  
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
  ClientConfig &getConfig()
  {
    return m_config;
  }
protected:
  std::streambuf* m_cerrbuf;
  std::streambuf* m_coutbuf;

  DOPE_SMARTPTR<NetStream> m_streamPtr;
};

#endif
