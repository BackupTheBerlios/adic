#include "player.h"

Player::PlayerDataPtr Player::m_defaultDataPtr;

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
  DOPE_CHECK(m_defaultDataPtr.get());
  m_r=m_defaultDataPtr->r*m_playerDataPtr->r;
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
  if (getX()) m_direction+=R(getX())*dt*m_defaultDataPtr->rotspeed*m_playerDataPtr->rotspeed;
  
  //! calculate new speed
  if (getY()) {
    m_speed+=V2D(0,1).rot(m_direction)*R(getY())*fit*m_defaultDataPtr->acceleration*m_playerDataPtr->acceleration*dt;
    m_fitness-=dt*1.0/(m_defaultDataPtr->endurance*m_playerDataPtr->endurance);
    m_fitness=std::max(m_fitness,R(m_defaultDataPtr->minfitness*m_playerDataPtr->minfitness));
  }
  if (m_speed==null) {
    // we are standing still
    if (m_fitness<1.0) m_fitness+=dt/(m_defaultDataPtr->recoverTime*m_playerDataPtr->recoverTime);
    return true;
  }
  
  // we are moving
  if (!getY()&&m_speed.length()<m_defaultDataPtr->minspeed*m_playerDataPtr->minspeed) {      
    m_speed=null;
    return true;
  }
  R maxs=m_defaultDataPtr->maxspeed*m_playerDataPtr->maxspeed*fit;
  if (m_speed.length()>maxs)
    (m_speed.normalize())*=maxs;
  // damp the speed not in the correct direction
  V2D hspeed(V2D(0,1).rot(m_direction).rot90().project(m_speed));
  m_speed-=hspeed*m_defaultDataPtr->hdamping*m_playerDataPtr->hdamping*dt;
  // damp the speed in the correct direction (only if up and down aren't pressed)
  if (!getY())
    m_speed-=V2D(0,1).rot(m_direction).project(m_speed)*m_defaultDataPtr->vdamping*m_playerDataPtr->vdamping*dt;
  m_pos+=m_speed*dt;
  return true;
}
