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

class Game
{
public:
  typedef std::vector<Player> Players;
  typedef std::vector<Icon> Icons;
  typedef std::vector<Door> Doors;
  typedef GenericPointer<Mesh, std::string, URILoader<URICache<Mesh> > > MeshPtr;
  typedef DOPE_SMARTPTR<World> WorldPtr;
  typedef std::vector<FWEdge::RoomID> PlayerRoomMap;

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

  void setWorldPtr(const WorldPtr &w);
  
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
  bool collideDoorAndPlayer(Door &d, PlayerID pid, bool rollbackdoor);
  
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
  FWEdge::RoomID calcPlayerInRoom(unsigned pid);
  
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
    TeamID tid(getTeamID(t));
    if (tid==TeamID(~0U))
      return NULL;
    assert(tid<m_teams.size());
    return &m_teams[tid];
  }

  TeamID getTeamID(const std::string &t) const
  {
    std::vector<Team>::const_iterator it(find(m_teams.begin(),m_teams.end(),t));
    if (it!=m_teams.end())
      return it-m_teams.begin();
    return ~0U;
  }

  TeamID getTeamIDofPlayer(PlayerID pid) const;
  
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

  void setPlayerName(PlayerID id,const std::string &name);

  PlayerID getPlayerID(const std::string &pn) const 
  {
    PlayerID pid=0U;
    for (;pid<m_playerNames.size();++pid) {
      if (m_playerNames[pid]==pn)
	return pid;
    }
    return ~0U;
  }
  const std::vector<std::string> &getPlayerNames() const
  {
    return m_playerNames;
  }
  void setPlayerNames(const std::vector<std::string> &playerNames)
  {
    m_playerNames=playerNames;
  }
  //! get team statistics
  /*!    
    \note is not const because playerIsLocked isn't
  */
  const std::vector<TeamStat> &getTeamStat();

  //! calculate team statistics
  std::vector<TeamStat> calcTeamStat();

  //! get the winning team
  /*!
    \param winner is only set if there is a winner

    returns - 0 if the game continues
            - 1 if there is a winner
	    - 2 if it ended in a draw
  */
  int getWinner(TeamID &teamID);
  
  //! do everything needed for a restart
  void restart();
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

  std::vector<std::vector<unsigned> > m_roomDoors;
  
  //! all closed rooms
  std::vector<FWEdge::RoomID> m_closedRooms;

  // data not pickled
  //! player names
  std::vector<std::string> m_playerNames;
  //! teams
  std::vector<Team> m_teams;

  //! team statistics
  std::vector<TeamStat> m_teamStat;
};
DOPE_CLASS(Game);

template <typename Layer2>
inline void composite(Layer2 &layer2, Game &g)
{
  g.composite(layer2);
}

#endif
