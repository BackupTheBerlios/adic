#include "player.h"

const R Player::m_maxspeed=20;
const R Player::m_acceleration=5;
const R Player::m_hdamping=2;
const R Player::m_vdamping=3;

Player::Player(const V2D &pos)
  : RoundObject(pos), m_direction(0), m_ix(0), m_iy(0), m_oldDirection(0)
{
}

bool
Player::step(R dt)
{
  // normally i use a different coord system
  // this one has y up x right and rotates clockwise
  m_pos+=m_speed*dt;
  m_speed+=V2D(0,1).rot(m_direction)*R(getY())*m_acceleration*dt;
  if (m_speed.length()>m_maxspeed)
    (m_speed.normalize())*=m_maxspeed;
  // damp the speed not in the correct direction
  m_speed-=V2D(0,1).rot(m_direction).rot90().project(m_speed)*m_hdamping*dt;
  // damp the speed in the correct direction
  if (!getY())
    m_speed-=V2D(0,1).rot(m_direction).project(m_speed)*m_vdamping*dt;
  m_direction+=R(getX())*dt;
  return true;
}
