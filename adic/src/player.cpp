#include "player.h"

Player::Player()
  : m_direction(0), m_ix(0), m_iy(0),
    m_fitness(1.0)
  //, m_oldDirection(0)
{
}

Player::Player(const V2D &pos, R dir, const std::string &playerDataURI)
  : RoundObject(pos,0), m_direction(dir), m_ix(0), m_iy(0), m_playerDataPtr(playerDataURI),
    m_fitness(1.0)
  //, m_oldDirection(0)
{
  DOPE_CHECK(m_playerDataPtr.get());
  m_r=m_playerDataPtr->r;
}

bool
Player::step(R dt)
{
  assert(m_playerDataPtr.get());
  V2D null(0,0);

  // normally i use a different coord system
  // this one has y up x right and rotates clockwise

  // usable fitness
  R fit=m_fitness;
  if (fit>1.0) fit=1.2;

  // calculate direction
  if (getX()) m_direction+=R(getX())*dt*m_playerDataPtr->rotspeed;
  
  //! calculate new speed
  if (getY()) {
    m_speed+=V2D(0,1).rot(m_direction)*R(getY())*fit*m_playerDataPtr->acceleration*dt;
    m_fitness-=dt*1.0/m_playerDataPtr->endurance;
    m_fitness=std::max(m_fitness,R(m_playerDataPtr->minfitness));
  }
  if (m_speed==null) {
    // we are standing still
    if (m_fitness<1.0) m_fitness+=dt/m_playerDataPtr->recoverTime;
    return true;
  }
  
  // we are moving
  if (!getY()&&m_speed.length()<R(0.5)) {      
    m_speed=null;
    std::cerr << "\nStopped player\n";
    return true;
  }
  R maxs=m_playerDataPtr->maxspeed*fit;
  if (m_speed.length()>maxs)
    (m_speed.normalize())*=maxs;
  // damp the speed not in the correct direction
  V2D hspeed(V2D(0,1).rot(m_direction).rot90().project(m_speed));
  m_speed-=hspeed*m_playerDataPtr->hdamping*dt;
  // damp the speed in the correct direction (only if up and down aren't pressed)
  if (!getY())
    m_speed-=V2D(0,1).rot(m_direction).project(m_speed)*m_playerDataPtr->vdamping*dt;
  m_pos+=m_speed*dt;
  return true;
}
