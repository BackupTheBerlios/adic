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
#include "team.h"

class PlayerInput;

//! door in the world coord system
/*!
  Why Door and RealDoor ?
  Door is pickled - RealDoor isn't
  => Door contains only the minimum information
  => RealDoor calculates information based on Door
*/
class RealDoor
{
  Door &d;
  V2D s;
  V2D e;
  R mass;
public:
  RealDoor(Door &_d, const V2D &_s, const V2D &_e);

  Wall asWall()
  {
    Wall r(Line(s,s+(e-s).rot(d.getAngle())));
    return r;
  }
  //! get speed at specific point of door
  /*!
    \todo in fact we want to get the momentum - and this should be /dist ?
  */
  V2D getImpuls(R dist);

  //! apply impuls at specific point of door
  void applyImpuls(R dist, V2D impuls);
};

class RealRoom
{
  const World &w;
  typedef std::map<FWEdge::EID,unsigned> Dmap;
  Dmap dmap;
  const std::vector<Door> &allDoors;
  FWEdge::RoomID roomID;
public:
  RealRoom(const World& _w, const std::vector<Door> &_allDoors, FWEdge::RoomID _roomID)
    : w(_w), allDoors(_allDoors), roomID(_roomID)
  {
    for (World::EdgeIterator i(w,roomID);i!=World::EdgeIterator(w);++i)
      {
	if ((*i).isDoor())
	  {
	    unsigned doorID=~0U;
	    for (unsigned d=0;d<allDoors.size();++d)
	      {
		if (allDoors[d].getEdgeID()==i.getID()) {
		  doorID=d;
		  break;
		}
	      }
	    DOPE_CHECK(doorID!=~0U);
	    dmap[i.getID()]=doorID;
	  }
      }
  }
  
  //! any door is closed ?
  bool getADIC() const
  {
    for (World::EdgeIterator i(w,roomID);i!=World::EdgeIterator(w);++i)
      {
	if ((*i).isDoor())
	  {
	    Dmap::const_iterator it(dmap.find(i.getID()));
	    DOPE_CHECK(it!=dmap.end());
	    unsigned d=it->second;
	    if (!allDoors[d].isClosed())
	      return false;
	  }
      }
    return true;
  }
};

class Game
{
public:
  typedef std::vector<Player> Players;
  typedef std::vector<Icon> Icons;
  typedef std::vector<Door> Doors;
  typedef GenericPointer<Mesh, std::string, URILoader<URICache<Mesh> > > MeshPtr;
  typedef DOPE_SMARTPTR<World> WorldPtr;
  typedef std::map<unsigned,FWEdge::RoomID> PlayerRoomMap;

  //! signal emitted if a crash occurs
  /*!
    passes the position and strength of the collision
  */
  SigC::Signal2<void, V2D, R> collision;

  Game() 
  {init();}
  Game(const std::string &meshURI);
  
  ~Game(){}

  void init();
  
  //! step dt seconds forward in time
  bool step(R dt);

  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(m_players).SIMPLE(m_icons).SIMPLE(m_doors).SIMPLE(m_meshPtr).SIMPLE(m_timeStamp);
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

  void setWorldPtr(WorldPtr &w);
  
  const Players &getPlayers() const
  {
    return m_players;
  }
  //! add a new player
  PlayerID addPlayer(const std::string &name, const std::string &URI);

  //! add a object
  void addObject(const Object &o);
  
  void setInput(const PlayerInput &i);

  void replace(Game &o);
  
  //! collide player with world (players, walls, doors)
  /*!
    \param test is true if you only want to test it - and don't want to emit the collision signal
  */
  bool collidePlayer(unsigned pid, bool test=false);
  
  //! collide door and player
  /*!
    \param rollbackdoor if true the door is rollbacked if false the player is rollbacked on collision

    \return true on collision otherwise false
  */
  bool collideDoorAndPlayer(Door &d, Player &p, bool rollbackdoor);
  
  //! return corresponding RealDoor to a door
  RealDoor doorInWorld(Door &d);
  
  Doors &getDoors()
  {
    return m_doors;
  }

  const TimeStamp &getTimeStamp()
  {
    return m_timeStamp;
  }

  //! calculate in which room this player is in
  void calcPlayerInRoom(unsigned pid);
  
  //! in which room is this player ?
  FWEdge::RoomID playerInRoomCached(unsigned pid);
  
  //! in which room is this player ?
  /*!
    \note This is slow - use the map instead
    \param p the player
  */
  FWEdge::RoomID playerInRoom(Player &p);

  const std::vector<FWEdge::RoomID> &getClosedRooms() const
  {
    return m_closedRooms;
  }
  bool roomIsClosed(FWEdge::RoomID r) const
  {
    return (find(m_closedRooms.begin(),m_closedRooms.end(),r)!=m_closedRooms.end());
  }
  bool playerIsLocked(unsigned p)
  {
    return roomIsClosed(playerInRoomCached(p));
  }
  //! get team by name
  /*!
    \return Pointer to team or NULL if it doesn't exist
  */
  Team *getTeam(const std::string &t)
  {
    std::vector<Team>::iterator it(find(m_teams.begin(),m_teams.end(),t));
    if (it!=m_teams.end())
      return &(*it);
    return NULL;
  }
  TeamID getTeamIDofPlayer(PlayerID pid);
  
  //! get team by ID
  Team *getTeam(unsigned id)
  {
    DOPE_CHECK(id<m_teams.size());
    return &m_teams[id];
  }
  
  Team* addTeam(const std::string &t, unsigned tno)
  {
    m_teams.push_back(Team(t,tno));
    return &m_teams.back();
  }

  unsigned numTeams() const
  {
    return m_teams.size();
  }
  const std::vector<Team> &getTeams() const
  {
    return m_teams;
  }
  void setTeams(const std::vector<Team> &teams)
  {
    m_teams=teams;
  }

  void setPlayerName(PlayerID id,const std::string &name)
  {
    if (id>=m_playerNames.size())
      m_playerNames.resize(id+1);
    m_playerNames[id]=name;
  }
  const std::vector<std::string> &getPlayerNames() const
  {
    return m_playerNames;
  }
  void setPlayerNames(const std::vector<std::string> &playerNames)
  {
    m_playerNames=playerNames;
  }
protected:
  bool miniStep(R dt);
  void calcClosedRooms();

  Players m_players;
  Icons m_icons;
  Doors m_doors;
  MeshPtr m_meshPtr;
  //! pointer to world
  WorldPtr m_worldPtr;
  TimeStamp m_timeStamp;

  //! accumulator
  R m_stepFault;

  //! players where in this room
  PlayerRoomMap m_playerRoomMap;

  //! all closed rooms
  std::vector<FWEdge::RoomID> m_closedRooms;

  // data not pickled
  //! player names
  std::vector<std::string> m_playerNames;
  //! teams
  std::vector<Team> m_teams;
};
DOPE_CLASS(Game);

template <typename Layer2>
inline void composite(Layer2 &layer2, Game &g)
{
  g.composite(layer2);
}

#endif
