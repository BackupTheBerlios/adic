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

void
Game::calcPlayerInRoom(unsigned p)
{
  WorldPtr w(getWorldPtr());
  if (!w.get()) return;
  FWEdge::RoomID r=FWEdge::noRoom;
  PlayerRoomMap::iterator it(m_playerRoomMap.find(p));
  if ((it!=m_playerRoomMap.end())&&(w->isInRoom(m_players[p].m_pos,it->second)))
    r=it->second;
  else
    r=playerInRoom(m_players[p]);
  if (r!=FWEdge::noRoom)
    m_playerRoomMap[p]=r;
  return;
}

FWEdge::RoomID 
Game::playerInRoomCached(unsigned pid)
{
  DOPE_CHECK(pid<m_players.size());
  PlayerRoomMap::iterator it(m_playerRoomMap.find(pid));
  if (it==m_playerRoomMap.end()) {
    calcPlayerInRoom(pid);
    it=m_playerRoomMap.find(pid);
    DOPE_CHECK(it!=m_playerRoomMap.end());
  }
  return it->second;
}

FWEdge::RoomID
Game::playerInRoom(Player &p)
{
  WorldPtr w(getWorldPtr());
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
  DOPE_CHECK(pid<m_players.size());
  Player &p=m_players[pid];
  WorldPtr w(getWorldPtr());
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
	  DOPE_CHECK(!p.collide(m_players[o],cv));
	  
	  if (test)
	    return true;
	  // increase fitness if team members
	  unsigned t1=getTeamIDofPlayer(pid);
	  if ((t1!=~0U)&&(t1==getTeamIDofPlayer(o))) {
	    m_players[pid].setFitness();
	    m_players[o].setFitness();
	  }
	  
	  // now calculate impuls
	  V2D oimp(cv.project(p.getSpeed()));
	  V2D timp((-cv).project(m_players[o].getSpeed()));
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
	  DOPE_CHECK(!w->collide(p,r,cv));
	  V2D imp(cv.project(p.getSpeed()));
	  p.applyImpuls(imp*-2);
	  p.commit();
	  if (!test) collision.emit(p.m_pos,2*imp.length());
	  return true;
	}
    }
    // 3. collide with doors
    for (unsigned d=0;d<m_doors.size();++d)
      {
	if (collideDoorAndPlayer(m_doors[d],p,false))
	  {
	    DOPE_CHECK(!collideDoorAndPlayer(m_doors[d],p,false));
	    return true;
	  }
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
      WorldPtr w(getWorldPtr());
      if (w.get()) {
	// calculate player step
	if (playerIsLocked(p))
	  m_players[p].setControl(0,0);
	DOPE_CHECK(!collidePlayer(p));
	m_players[p].step(dt);
	calcPlayerInRoom(p);
	collidePlayer(p);
      }
      // "collide" collect icons
    }

  // step doors
  for (unsigned d=0;d<m_doors.size();++d)
    {
      m_doors[d].step(dt);
      if (m_doors[d].getSpeed()!=R(0)) {
	// collide with players
	for (unsigned p=0;p<m_players.size();++p)
	  {
	    if (collideDoorAndPlayer(m_doors[d],m_players[p],true))
	      {
		DOPE_CHECK(!collideDoorAndPlayer(m_doors[d],m_players[p],true));
		break;
	      }
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
  WorldPtr w(getWorldPtr());
  if (!w.get())
    return;
  for (unsigned r=0;r<w->getNumRooms();++r)
    {
      RealRoom room(*w.get(),m_doors,r);
      if (room.getADIC())
	m_closedRooms.push_back(r);
    }
}

PlayerID
Game::addPlayer(const std::string &name)
{
  PlayerID id=m_players.size();
  WorldPtr w(getWorldPtr());
  DOPE_CHECK(w.get());
  const Mesh::StartPoints &s(w->getStartPoints());
  for (unsigned p=0;p<s.size();++p) {
    Player newp(s[p].first,s[p].second*M_PI/180);
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
Game::addObject(const V2D &pos, R dir, const std::string &name, R r)
{
  Player newp(pos,dir*M_PI/180,1,r);
  PlayerID id=m_players.size();
  m_players.push_back(newp);
  DOPE_CHECK(!collidePlayer(id,true));
  setPlayerName(id,name);
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

Game::WorldPtr
Game::getWorldPtr()
{
  if (m_meshPtr.get()&&(!m_worldPtr.get()))
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
Game::setWorldPtr(WorldPtr &w)
{
  m_worldPtr=w;
}

RealDoor
Game::doorInWorld(Door &d)
{
  WorldPtr w(getWorldPtr());
  DOPE_CHECK(w.get());
  const FWEdge &e(w->getEdge(d.getEdgeID()));
  const V2D &sv=w->getPoint(e.m_sv);
  const V2D &ev=w->getPoint(e.m_ev);
  return RealDoor(d,sv,ev);
}
bool
Game::collideDoorAndPlayer(Door &d, Player &p, bool rollbackdoor)
{
  RealDoor rd(doorInWorld(d));
  Wall w(rd.asWall());
  V2D cv;
  R dist;
  if (!w.collide(p,cv,dist))
    return false;
  // todo apply imulses - unfortunately i don't know the distance of the
  // collision point => i can't calculate the momentum
  if (!rollbackdoor)
    p.rollback();
  else
    d.rollback();
  V2D oimp(cv.project(p.getSpeed()));
  // todo this seems a bit stupid since the speed of the door is always in the direction
  // of cv (no not really because of the pillars at the end of the door)
  // todo this is buggy
  V2D timp(cv.project(rd.getImpuls(dist)));
  p.applyImpuls(timp*0.9-oimp);
  rd.applyImpuls(dist,oimp*0.9-timp);
  p.commit();
  d.commit();
  collision.emit(p.m_pos,oimp.length()+timp.length());
  return true;
}

TeamID 
Game::getTeamIDofPlayer(PlayerID pid)
{
  if (!m_players[pid].isPlayer())
    return ~0U;
  // find team this player is in
  for (unsigned i=0;i<m_teams.size();++i) {
    std::vector<PlayerID>::const_iterator it(std::find(m_teams[i].playerIDs.begin(),m_teams[i].playerIDs.end(),pid));
    if (it!=m_teams[i].playerIDs.end())
      return i;
  }
  return ~0U;
}

