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
  \file bot.h
  \brief bot interface
  \author Jens Thiele
*/

#ifndef BOT_H
#define BOT_H

#include "botclient.h"

//! interface to bot clients - implement it if you write a bot client
/*
  for an example implementation take a look at KarmeBot (karmebot.h)
*/
class Bot : public SigC::Object
{
public:
  Bot(BotClient &_client) : client(_client)
  {}
  ~Bot(){}

  SigC::Signal1<void, Input &> input;
  SigC::Signal1<void, ChatMessage &> chatMessage;

  virtual void handleNewClient(DOPE_SMARTPTR<NewClient> mPtr){}
  virtual void handlePlayerCollision(PlayerID p1, PlayerID p2, const V2D &cv){}
  virtual void handleWallCollision(PlayerID p, const std::vector<FWEdge::EID> &eids, const V2D &cv){}
  virtual void handleDoorCollision(PlayerID p, unsigned did, const V2D &cv){}
  //! called at the beginning of a new game
  virtual void startGame(){}
  //! called at the end of a game
  virtual void endGame(){}
  
  virtual bool step(R dt)=0;

  BotClientConfig &getConfig() 
  {
    return client.getConfig();
  }
protected:
  BotClient &client;
};

class BotFactory
{
public:  
  Bot* create(BotClient &client);
};

#endif
