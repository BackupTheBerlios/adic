#include "game.h"
#include "input.h"

RealDoor::RealDoor(Door &_d, const V2D &_s, const V2D &_e)
  : d(_d), s(_s), e(_e), mass(0.1)
{}


V2D 
RealDoor::getImpuls(R dist)
{
  R tSpeed=d.getSpeed()*dist*2*M_PI*mass;
  // calculate normal
  V2D dv((e-s).rot(d.getAngle()));
  dv.rot90();
  dv.normalize();
  dv*=tSpeed;
  return dv;
}

void 
RealDoor::applyImpuls(R dist, V2D impuls)
{
  V2D n((e-s).rot(d.getAngle()));
  n.rot90();
  V2D i(n.project(impuls));
  R m=i.length();
  if (n.dot(i)<0)
    m=-m;
  d.addSpeed(m/(dist*2*M_PI*mass));
}

Game::Game(const std::string &meshURI) 
  : m_meshPtr(meshURI)
{
  init();
}

void
Game::init()
{
  m_stepFault=0;
}

bool
Game::step(R dt)
{
  dt+=m_stepFault;
  R stepSize=0.01;
  while (dt>stepSize)
    {
      bool r=miniStep(stepSize);
      if (!r)
	return false;
      dt-=stepSize;
    }
  m_stepFault=dt;
  return true;
}

FWEdge::RoomID
Game::calcPlayerInRoom(unsigned p)
{
  const WorldPtr &w(getWorldPtr());
  if (!w.get()) return FWEdge::noRoom;
  FWEdge::RoomID r=FWEdge::noRoom;
  while (p>=m_playerRoomMap.size()) {
    m_playerRoomMap.push_back(r);
  }
  r=m_playerRoomMap[p];
  if ((r!=FWEdge::noRoom)&&(w->isInRoom(m_players[p].m_pos,r)))
    return r;
  return (m_playerRoomMap[p]=playerInRoom(m_players[p]));
}

FWEdge::RoomID 
Game::playerInRoomCached(unsigned pid)
{
  assert(pid<m_players.size());
  if (pid>=m_playerRoomMap.size())
    return calcPlayerInRoom(pid);
  return m_playerRoomMap[pid];
}

FWEdge::RoomID
Game::playerInRoom(Player &p)
{
  const WorldPtr &w(getWorldPtr());
  if (!w.get()) {
    DOPE_WARN("\nreturned noRoom because I don't have the world yet\n");
    return FWEdge::noRoom;
  }
  FWEdge::RoomID r=w->inRoom(p.m_pos);
  DOPE_CHECK(r<w->getNumRooms());
  return r;
}

bool
Game::collidePlayer(unsigned pid, bool test)
{
  assert(pid<m_players.size());
  Player &p=m_players[pid];
  if (!p.moved())
    return false;

  const WorldPtr &w(getWorldPtr());
  if (w.get()) {
    // find room this player is in
    FWEdge::RoomID r=playerInRoomCached(pid);
    
    // now collide and rollback on collision
    // 1. collide with other players
    for (unsigned o=0;o<m_players.size();++o){
      if (&m_players[o]==&p) continue;
      
      V2D cv;
      if (p.collide(m_players[o],cv))
	{
	  p.rollback();
	  assert(!p.collide(m_players[o],cv));
	  
	  if (test)
	    return true;
	  // increase fitness if team members
	  TeamID t1=getTeamIDofPlayer(pid);
	  if ((t1!=TeamID(~0U))&&(t1==getTeamIDofPlayer(o))) {
	    m_players[pid].setFitness();
	    m_players[o].setFitness();
	  }
	  if ((m_players[pid].isPlayer()||m_players[o].isPlayer())&&
	      (m_players[pid].getType()==11)||(m_players[o].getType()==11)) {
	    
	    // increase fitness if it is a fountain (todo: either make a const or method and not 11)
	    if (m_players[o].getType()==11)
	      m_players[pid].setFitness();
	    else
	      m_players[o].setFitness();
	  }
	  
	  // now calculate impuls
	  V2D oimp(cv.project(p.getImpuls()));
	  V2D timp((-cv).project(m_players[o].getImpuls()));
	  p.applyImpuls(timp-oimp);
	  m_players[o].applyImpuls(oimp-timp);
	  p.commit();
	  m_players[o].commit();
	  collision.emit(p.m_pos,oimp.length()+timp.length());
	  return true;
	}
    }

    // 2. collide with walls
    if (r!=FWEdge::noRoom) {
      V2D cv;
      // collide with room
      if (w->collide(p,r,cv))
	{
	  p.rollback();
	  calcPlayerInRoom(pid);
	  r=playerInRoomCached(pid);
	  assert(!w->collide(p,r,cv));
	  V2D imp(cv.project(p.getImpuls()));
	  p.applyImpuls(imp*-2);
	  p.commit();
	  if (!test) collision.emit(p.m_pos,2*imp.length());
	  return true;
	}
    }
    // 3. collide with doors
    for (unsigned d=0;d<m_doors.size();++d)
      {
	if (collideDoorAndPlayer(m_doors[d],pid,false))
	  return true;
      }
  }
  p.commit();
  return false;
}


bool
Game::miniStep(R dt)
{
  m_timeStamp+=TimeStamp(dt);
  for (unsigned p=0;p<m_players.size();++p)
    {
      const WorldPtr &w(getWorldPtr());
      if (w.get()) {
	// calculate player step
	if (playerIsLocked(p))
	  m_players[p].setControl(0,0);
	assert(!collidePlayer(p));
	m_players[p].step(dt);
	if (m_players[p].moved()) {
	  calcPlayerInRoom(p);
	  collidePlayer(p);
	  assert(!collidePlayer(p));
	}
      }
    }

  // step doors
  for (unsigned d=0;d<m_doors.size();++d)
    {
      m_doors[d].step(dt);
      if (m_doors[d].moved()) {
	// collide with players
	for (unsigned p=0;p<m_players.size();++p)
	  {
	    if (collideDoorAndPlayer(m_doors[d],p,true))
	      break;
	  }
      }
      m_doors[d].commit();
    }
  calcClosedRooms();
  return true;
}

void Game::calcClosedRooms()
{
  m_closedRooms.clear();
  const WorldPtr &w(getWorldPtr());
  if (!w.get())
    return;
  unsigned nr=w->getNumRooms();
  if (nr>m_roomDoors.size()) {
    m_roomDoors.resize(nr);
    for (unsigned r=0;r<nr;++r) {
      for (World::EdgeIterator i(*w,r);i!=World::EdgeIterator(*w);++i)
	{
	  if ((*i).isDoor())
	    {
	      unsigned doorID=~0U;
	      unsigned eid=i.getID();
	      for (unsigned d=0;d<m_doors.size();++d)
		{
		  if (m_doors[d].getEdgeID()==eid) {
		    doorID=d;
		    break;
		  }
		}
	      DOPE_CHECK(doorID!=~0U);
	      m_roomDoors[r].push_back(doorID);
	    }
	}
    }
  }
  for (unsigned r=0;r<nr;++r) {
    unsigned nd=m_roomDoors[r].size();
    unsigned d=0;
    for (;d<nd;++d) {
      if (!m_doors[m_roomDoors[r][d]].isClosed())
	break;
    }
    if (d==nd) m_closedRooms.push_back(r);
  }
}

PlayerID
Game::addPlayer(const std::string &name, const std::string &URI)
{
  PlayerID id=m_players.size();
  const WorldPtr &w(getWorldPtr());
  assert(w.get());
  const Mesh::StartPoints &s(w->getStartPoints());
  for (unsigned p=0;p<s.size();++p) {
    Player newp(s[p].first,s[p].second*M_PI/180,URI);
    m_players.push_back(newp);
    if (!collidePlayer(id,true)) {
      setPlayerName(id,name);
      playerAdded.emit(id);
      return id;
    }else{
      m_players.pop_back();
    }
  }
  // did not find a start place
  return ~0U;
}

void 
Game::addObject(const Object &o)
{
  Player newp(o.pos,o.dir*M_PI/180,o.URI);
  PlayerID id=m_players.size();
  m_players.push_back(newp);
  DOPE_CHECK(!collidePlayer(id,true));
  //  setPlayerName(id,name);
}

void 
Game::setInput(const PlayerInput &i)
{
  if (i.id>=m_players.size())
    {
      DOPE_WARN("id out of range: "<<i.id<<">="<<m_players.size());
      return;
    }
  m_players[i.id].setControl(i.i.x,i.i.y);
}

void 
Game::replace(Game &o)
{
  /*  
      TimeStamp myTime(getTimeStamp());
      TimeStamp serverTime(o.getTimeStamp());
      // lag compensation
	 if (serverTime<myTime) {
	 // packet lag
	 TimeStamp lag(myTime-serverTime);
	 R dt=lag.getSec()+R(lag.getUSec())/1000000;
	 std::cerr << "\nLag: "<<dt<<" sec.\n";
	 o.step(dt);
    }*/
  // m_players
  m_players=o.m_players;
  for (unsigned p=0;p<m_players.size();++p) {
    m_players[p].commit();
    calcPlayerInRoom(p);
  }
  
  /*
    DOPE_CHECK(m_players.size()<=o.m_players.size());
    unsigned pmax=m_players.size();
    pmax=0; // todo
    for (unsigned p=0;p<pmax;++p) {
    todo bug: old values are consistent among each other but not with the new values
    a rollback might set a position with crash
    lösung ?
    einfach alle empfangenen positionen commiten ?
    => geht nur wenn fpu precision auf allen rechnern gleich 
    o.m_players[p].setOldValues(m_players[p]);
    m_players[p]=o.m_players[p];
    if (m_players[p].moved())
    calcPlayerInRoom(p);
    }
    if (m_players.size()<o.m_players.size()) {
    m_players.resize(o.m_players.size());
    for (unsigned p=pmax;p<o.m_players.size();++p) {
    m_players[p]=o.m_players[p];
    m_players[p].commit();
    calcPlayerInRoom(p);
    }
    }*/

  m_doors=o.m_doors;
  calcClosedRooms();
  m_meshPtr=o.m_meshPtr;
  m_timeStamp=o.m_timeStamp;
}


Game::WorldPtr
Game::getWorldPtr()
{
  if ((!m_worldPtr.get())&&m_meshPtr.get())
    {
      //      DOPE_WARN("reached");
      m_worldPtr=WorldPtr(new World(*m_meshPtr.get()));
      DOPE_CHECK(m_worldPtr.get());
      // get all doors
      if (m_doors.empty()) {
	std::vector<FWEdge::EID> d(m_worldPtr->getAllDoors());
	std::cerr << "\nWorld has "<<d.size()<<"doors\n";
	for (unsigned i=0;i<d.size();++i)
	  {
	    m_doors.push_back(Door(d[i]));
	  }
      }
    }
  return m_worldPtr;
}

void
Game::setWorldPtr(const WorldPtr &w)
{
  m_worldPtr=w;
}

RealDoor
Game::doorInWorld(Door &d)
{
  const WorldPtr &w(getWorldPtr());
  DOPE_CHECK(w.get());
  const FWEdge &e(w->getEdge(d.getEdgeID()));
  const V2D &sv=w->getPoint(e.m_sv);
  const V2D &ev=w->getPoint(e.m_ev);
  return RealDoor(d,sv,ev);
}
bool
Game::collideDoorAndPlayer(Door &d, PlayerID pid, bool rollbackdoor)
{
  // check if player and door are in the same room
  const WorldPtr &wp(getWorldPtr());
  DOPE_CHECK(wp.get());
  FWEdge::RoomID rooms[2];
  wp->getRoomIDs(d.getEdgeID(),rooms);
  FWEdge::RoomID proom(playerInRoomCached(pid));
  if ((proom!=rooms[0])&&(proom!=rooms[1]))
    return false;

  Player &p(m_players[pid]);
  RealDoor rd(doorInWorld(d));
  Wall w(rd.asWall());
  V2D cv;
  R dist;
  if (!w.collide(p,cv,dist))
    return false;

  // todo remove
  V2D preRollback=p.m_pos;

  if (!rollbackdoor)
    p.rollback();
  else
    d.rollback();

  // todo remove
  V2D postRollback=p.m_pos;
  V2D diff=preRollback-postRollback;

  // player impuls
  V2D pimp(cv.project(p.getImpuls()));
  // this seems a bit stupid because the speed of the door should always be 
  // in the direction of cv but it is not because of the pillars at the end of the door
  // door impuls
  V2D dimp(cv.project(rd.getImpuls(dist)));
  // special case here
  // we want to be able to block doors with barrels
  if (!p.isPlayer())
    dimp*=0.1;
  p.applyImpuls(dimp*0.9-pimp);
  rd.applyImpuls(dist,pimp*0.9-dimp);
  p.commit();
  d.commit();
  collision.emit(p.m_pos,pimp.length()+dimp.length());
  // todo remove it again
  {
    RealDoor rd(doorInWorld(d));
    Wall w(rd.asWall());
    V2D cv;
    R dist;
    if (w.collide(p,cv,dist)) {
      DOPE_FATAL("rollback failed - diff is: "<<diff);
    }
  }
  return true;
}

TeamID 
Game::getTeamIDofPlayer(PlayerID pid) const
{
  if (!m_players[pid].isPlayer())
    return ~0U;
  // find team this player is in
  for (unsigned i=0;i<m_teams.size();++i) {
    std::vector<PlayerID>::const_iterator it(std::find(m_teams[i].playerIDs.begin(),m_teams[i].playerIDs.end(),pid));
    if (it!=m_teams[i].playerIDs.end())
      return TeamID(i);
  }
  return ~0U;
}

void
Game::setPlayerName(PlayerID id,const std::string &name)
{
  if (id>=m_playerNames.size())
    m_playerNames.resize(id+1);
  std::string rname=name;
  unsigned c=2;
  while (std::find(m_playerNames.begin(),m_playerNames.end(),rname)!=m_playerNames.end()) {
    rname=name+"["+anyToString(c)+"]";
    ++c;
  }
  m_playerNames[id]=rname;
}

const std::vector<TeamStat> &
Game::getTeamStat()
{
  m_teamStat=calcTeamStat();
  return m_teamStat;
}

std::vector<TeamStat>
Game::calcTeamStat()
{
  std::vector<TeamStat> res(m_teams.size());
  for (unsigned i=0;i<m_teams.size();++i) {
    TeamStat stat;
    stat.numPlayers=m_teams[i].playerIDs.size();
    stat.locked=0;
    for (unsigned j=0;j<stat.numPlayers;++j) {
      unsigned id=m_teams[i].playerIDs[j];
      // perhaps we did not receive this player yet
      if (id<m_players.size()&&playerIsLocked(id))
	++stat.locked;
    }
    res[i]=stat;
  }
  return res;
}

int
Game::getWinner(TeamID &teamID) 
{
  if (getTeams().size()<2)
    return 0;

  TeamID noWin=TeamID(~0U);
  TeamID winTeam=noWin;
  const std::vector<TeamStat> &teamStat(getTeamStat());
  for (unsigned i=0;i<teamStat.size();++i) {
    if (!teamStat[i].allLocked()) {
      if (winTeam!=noWin)
	// second team with unlocked players => no win
	return 0;
      //else
      winTeam=i;
    }
  }
  if (winTeam==noWin)
    return 2; // all teams are locked
  teamID=winTeam;
  return 1;
}

void
Game::restart()
{
  m_players.clear();
  m_icons.clear();
  m_doors.clear();
  m_stepFault=0;
  m_playerRoomMap.clear();
  m_roomDoors.clear();
  m_closedRooms.clear();
  m_playerNames.clear();
  m_teams.clear();
  setWorldPtr(WorldPtr());
}
