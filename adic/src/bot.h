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

#include <sigc++/signal_system.h>
#include "botclient.h"

//! Bot interface - implement it if you write a bot
/*
  for an example implementation take a look at KarmeBot (karmebot.h)
*/
class Bot : public SigC::Object
{
public:
  Bot(BotClient &_client, PlayerID _pid, unsigned _inputID) 
    : client(_client), pid(_pid), inputID(_inputID)
  {}
  
  virtual ~Bot(){}

  SigC::Signal1<void, Input &> input;
  SigC::Signal1<void, ChatMessage &> chatMessage;

  //! is called if the bot collided with another player or player-like object
  virtual void playerCollision(PlayerID cp, const V2D &cv){}
  //! is called if the bot collided with one or more walls
  virtual void wallCollision(const std::vector<FWEdge::EID> &eids, const V2D &cv){}
  //! is called if the bot collided with a door
  virtual void doorCollision(unsigned did, const V2D &cv){}
  //! do your work here
  /*!
    \param dt the time elapsed
  */
  virtual bool step(R dt){return true;}
protected:
  //! send input state to server (if changed)
  void sendInput();

  BotClient& client;
  //! the player id of the bot
  PlayerID pid;
  //! the input id
  unsigned inputID;
  //! current input state
  Input cinput;
  //! input state send to the server
  Input oinput;

  //! calculates direction of given vector
  /*!
    \return direction (0<=direction<2*Pi)
   */
  static R vdir(const V2D &v)
  {
    // our coord sys is a little bit wired (we have clockwise angles and start from 0/1)
    // atan2 is between -M_PI and +M_PI
    R res=M_PI/2-atan2(v[1],v[0]);
    // res is now between -M_PI/2 and M_PI+M_PI/2
    return (res>=0) ? res : res+M_PI*2;
  }
};

//! create a new bot
class BotFactory
{
public:  
  Bot* create(BotClient &client,PlayerID _pid, unsigned _inputID);
};

#endif
