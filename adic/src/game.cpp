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

/*!
  \todo don't calculate it on each call - only calculate it once per step
 */
FWEdge::RoomID
Game::playerInRoom(unsigned p)
{
  WorldPtr w(getWorldPtr());
  if (!w.get())
    return FWEdge::noRoom;
  FWEdge::RoomID r=FWEdge::noRoom;
  PlayerRoomMap::iterator it(m_playerRoomMap.find(p));
  if (it!=m_playerRoomMap.end()) {
    if (w->isInRoom(m_players[p].m_pos,it->second))
      r=it->second;
  }
  if (r==FWEdge::noRoom)
    r=w->inRoom(m_players[p].m_pos);
  if (r!=FWEdge::noRoom)
    m_playerRoomMap[p]=r;
  return r;
}

bool
Game::miniStep(R dt)
{
  // not problematic because a signed int (32bit) can keep 2147*10^6 usec => 2147 sec.
  m_timeStamp+=TimeStamp(dt);
  bool collided=false;
  for (unsigned p=0;p<m_players.size();++p)
    {
      WorldPtr w(getWorldPtr());
      if (w.get()) {
	// find room this player is in
	FWEdge::RoomID r=playerInRoom(p);

	// calculate player step
	if (roomIsClosed(r))
	  m_players[p].setControl(0,0);

	m_players[p].step(dt);
	// now collide and rollback on collision
	// 1. collide with other players
	for (unsigned o=0;o<m_players.size();++o){
	  if (o==p) continue;
	  V2D cv;
	  if (m_players[p].collide(m_players[o],cv))
	    {
	      m_players[p].rollback();
	      DOPE_CHECK(!m_players[p].collide(m_players[o],cv));
	      
	      // now calculate impuls
	      V2D oimp(cv.project(m_players[p].getSpeed()));
	      V2D timp((-cv).project(m_players[o].getSpeed()));
	      m_players[p].applyImpuls(timp-oimp);
	      m_players[o].applyImpuls(oimp-timp);
	      m_players[p].commit();
	      m_players[o].commit();
	      collided=true;
	      break;
	    }
	}
	// we rolled back this player -> there can't be any collision any moor
	if (collided) continue;

	// 2. collide with walls
	if (r!=FWEdge::noRoom) {
	  V2D cv;
	  // collide with room
	  if (w->collide(m_players[p],r,cv))
	    {
	      m_players[p].rollback();
	      DOPE_CHECK(!w->collide(m_players[p],r,cv));
	      collided=true;
	      V2D imp(cv.project(m_players[p].getSpeed()));
	      m_players[p].applyImpuls(imp*-2);
	      m_players[p].commit();
	    }
	}
	if (collided) continue;
	// 3. collide with doors
	for (unsigned d=0;d<m_doors.size();++d)
	  {
	    if (collideDoorAndPlayer(m_doors[d],m_players[p],false))
	      {
		DOPE_CHECK(!collideDoorAndPlayer(m_doors[d],m_players[p],false));
		collided=true;
		break;
	      }
	  }
	if (collided) continue;
      }
      // "collide" collect icons

      // commit move because there was no collision
      m_players[p].commit();
    }

  // step doors
  for (unsigned d=0;d<m_doors.size();++d)
    {
      m_doors[d].step(dt);
      // collide with players
      bool collided=false;
      for (unsigned p=0;p<m_players.size();++p)
	{
	  if (collideDoorAndPlayer(m_doors[d],m_players[p],true))
	    {
	      DOPE_CHECK(!collideDoorAndPlayer(m_doors[d],m_players[p],true));
	      collided=true;
	      break;
	    }
	}
      if (collided) continue;
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
      // select room color
      RealRoom room(*w.get(),m_doors,r);
      if (room.getADIC())
	m_closedRooms.push_back(r);
    }
}

void 
Game::setInput(const PlayerInput &i)
{
  if (i.id>=m_players.size())
    {
      DOPE_WARN("id out of range");
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
  p.applyImpuls(timp-oimp);
  rd.applyImpuls(dist,oimp-timp);
  p.commit();
  d.commit();
  return true;
}
