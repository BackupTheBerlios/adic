#include "game.h"
#include "input.h"

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

bool
Game::miniStep(R dt)
{
  // not problematic because a signed int (32bit) can keep 2147*10^6 usec => 2147 sec.
  m_timeStamp+=TimeStamp(dt);
  bool collided=false;
  for (unsigned p=0;p<m_players.size();++p)
    {
      // calculate step
      m_players[p].step(dt);
      // now collide and rollback on collision
      // 1. collide with other players
      for (unsigned o=0;o<m_players.size();++o){
	if (o==p) continue;
	V2D cv;
	if (m_players[p].collide(m_players[o],cv))
	  {
	    m_players[p].rollback();
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
      if (!collided) {
	WorldPtr w(getWorldPtr());
	if (w.get()) {
	  // 2. collide with walls
	  FWEdge::RoomID r=w->inRoom(m_players[p].m_pos);
	  if (r!=FWEdge::noRoom) {
	    V2D cv;
	    if (w->collide(m_players[p],r,cv))
	      {
		m_players[p].rollback();
		collided=true;
		V2D imp(cv.project(m_players[p].getSpeed()));
		m_players[p].applyImpuls(imp*-2);
		m_players[p].commit();
	      }
	  }
	  if (!collided) {
	    // 3. collide with doors
	    for (unsigned d=0;d<m_doors.size();++d)
	      {
		if (collideDoorAndPlayer(m_doors[d],m_players[p]))
		  {
		    collided=true;
		    break;
		  }
	      }
	    if (!collided)
	      m_players[p].commit();
	  }

	}
      }
      // "collide" collect icons
    }
  // step doors
  for (unsigned d=0;d<m_doors.size();++d)
    {
      m_doors[d].step(dt);
      // collide with players
      bool collided=false;
      for (unsigned p=0;p<m_players.size();++p)
	{
	  if (collideDoorAndPlayer(m_doors[d],m_players[p]))
	    {
	      collided=true;
	      break;
	    }
	}
      if (!collided)
	m_doors[d].commit();
    }
  return true;
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
      DOPE_WARN("reached");
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
Game::collideDoorAndPlayer(Door &d, Player &p)
{
  RealDoor rd(doorInWorld(d));
  Wall w(rd.asWall());
  V2D cv;
  R dist;
  if (!w.collide(p,cv,dist))
    return false;
  // todo apply imulses - unfortunately i don't know the distance of the
  // collision point => i can't calculate the momentum
  p.rollback();
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
