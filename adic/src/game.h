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
   \file Game.h
   \brief the game - the simulator
   \author Jens Thiele
*/

#ifndef GAME_H
#define GAME_H

#include "world.h"
#include "player.h"
#include "door.h"
#include "icon.h"
#include "uriloader.h"

class Game
{
public:
  typedef uint32_t PlayerID;
  typedef std::vector<Player> Players;
  typedef std::vector<Icon> Icons;
  typedef std::vector<Door> Doors;
  typedef GenericPointer<World, std::string, URILoader<URICache<World> > > WorldPtr;
  
  Game(){}
  Game(const std::string &worldURI) : m_worldPtr(worldURI)
  {}
  
  ~Game(){}

  //! step dt seconds forward in time
  bool step(R dt);

  //! add a new player
  PlayerID addPlayer();

  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(m_players).SIMPLE(m_icons).SIMPLE(m_doors);
  }

  SigC::Signal1<void,PlayerID> playerAdded;
protected:
  Players m_players;
  Icons m_icons;
  Doors m_doors;
  WorldPtr m_worldPtr;
};
DOPE_CLASS(Game);

template <typename Layer2>
inline void composite(Layer2 &layer2, Game &g)
{
  g.composite(layer2);
}

#endif
