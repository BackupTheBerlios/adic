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
#include "wall.h"

class PlayerInput;

//! door in the world coord system
class RealDoor
{
  Door &d;
  V2D s;
  V2D e;
  R mass;
public:
  RealDoor(Door &_d, const V2D &_s, const V2D &_e)
    : d(_d), s(_s), e(_e), mass(0.01)
  {}
  Wall asWall()
  {
    Wall r(Line(s,s+(e-s).rot(d.getAngle())));
    return r;
  }
  //! get speed at specific point of door
  /*!
    \todo in fact we want to get the momentum - and this should be /dist ?
  */
  V2D getImpuls(R dist)
  {
    R tSpeed=d.getSpeed()*dist*2*M_PI*mass;
    // calculate normal
    V2D dv((e-s).rot(d.getAngle()));
    dv.rot90();
    dv.normalize();
    dv*=tSpeed;
    return dv;
  }
  //! apply impuls at specific point of door
  void applyImpuls(R dist, V2D impuls)
  {
    V2D n((e-s).rot(d.getAngle()));
    n.rot90();
    V2D i(n.project(impuls));
    R m=i.length();
    if (n.dot(i)<0)
      m=-m;
    d.addSpeed(m/(dist*2*M_PI*mass));
  }
};


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
    if meshptr != NULL but worldPtr==NULL the world and doors will be initialized

    \returns worldPtr - may be a NULL pointer
  */
  WorldPtr getWorldPtr();

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

  //! collide door and player
  /*!
    \param rollbackdoor if true the door is rollbacked if false the player is rollbacked on collision

    \return true on collision otherwise false
  */
  bool collideDoorAndPlayer(Door &d, Player &p);
  
  //! return corresponding RealDoor to a door
  RealDoor doorInWorld(Door &d);
  
  Doors &getDoors()
  {
    return m_doors;
  }
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
