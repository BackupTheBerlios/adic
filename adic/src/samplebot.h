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
  \file samplebot.h
  \brief class SampleBot
  \author Jens Thiele
*/

#ifndef SAMPLEBOT_H
#define SAMPLEBOT_H

#include "bot.h"

//! sample bot implementing Bot interface
/*!
  simple stupid sample implementation of a bot
  to demonstrate how it works
*/
class SampleBot : public Bot
{
public:
  SampleBot(BotClient &_client, PlayerID _pid, unsigned _inputID);
  
  bool step(R dt);
  void playerCollision(PlayerID cp, const V2D &cv);
  void wallCollision(const std::vector<FWEdge::EID> &eids, const V2D &cv);
  void doorCollision(unsigned did, const V2D &cv);

protected:  
  //! try to rotate to wished direction - by setting cinput as needed
  void reachDir();
  //! try to follow rabbit
  void follow();
  
  //! wished direction
  R dir;
  //! follow mode ?
  bool followMode;
  //! the id of the player we follow
  PlayerID rabbit;

  //! we accept a direction +-threshold
  static const R threshold;
};

#endif
