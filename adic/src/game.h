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

class PlayerInput;

class Game
{
public:
  typedef uint16_t PlayerID;
  typedef std::vector<Player> Players;
  typedef std::vector<Icon> Icons;
  typedef std::vector<Door> Doors;
  typedef GenericPointer<Mesh, std::string, URILoader<URICache<Mesh> > > MeshPtr;
  typedef DOPE_SMARTPTR<World> WorldPtr;
  
  Game(){}
  Game(const std::string &meshURI) : m_meshPtr(meshURI)
  {}
  
  ~Game(){}

  //! step dt seconds forward in time
  bool step(R dt);

  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(m_players).SIMPLE(m_icons).SIMPLE(m_doors).SIMPLE(m_meshPtr);
    // karme 2002-05-30:
    // m_worldPtr is not listed here because I did not solve the "reentrance" problem in DOPE++ yet
    // (s.a. dope TODO and xmlsaxinstream.h) and i pickle m_meshPtr instead to work around the problem
  }

  SigC::Signal1<void,PlayerID> playerAdded;

  //! get World Ptr
  /*!
    \returns worldPtr - may be a NULL pointer
  */
  WorldPtr getWorldPtr()
  {
    if (m_meshPtr.get()&&(!m_worldPtr.get()))
      m_worldPtr=WorldPtr(new World(*m_meshPtr.get()));
    return m_worldPtr;
  }
  const Players &getPlayers() const
  {
    return m_players;
  }
  //! add a new player
  PlayerID addPlayer()
  {
    PlayerID id=m_players.size();
    Player newp(V2D(50,50));
    m_players.push_back(newp);
    playerAdded.emit(id);
    return id;
  }

  void setInput(const PlayerInput &i);

protected:
  Players m_players;
  Icons m_icons;
  Doors m_doors;
  MeshPtr m_meshPtr;
  //! pointer to world
  WorldPtr m_worldPtr;
};
DOPE_CLASS(Game);

template <typename Layer2>
inline void composite(Layer2 &layer2, Game &g)
{
  g.composite(layer2);
}

#endif
