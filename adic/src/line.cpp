#include "line.h"

R Line::project(const V2D &p)
{
  //return (m_a - p)*(m_a-m_b)/(m_a-m_b).norm2();
  return (m_b-m_a).rot90()*(p-m_a);
}


R Line::dist(const V2D &p)
{
  // return sqrt((m_a - p)*(m_a - p) - project(p)*project(p));
  return (m_b-m_a).rot90()*(p-m_a);
}

bool Line::collide(const Line &o)
{
  return (m_vn == o.m_vn);
}

bool Line::collide(const Line &o, R &cp)
{
  if (collide(o))
  {
    cp = ((m_b-m_a).rot90()*(m_a-o.m_a)) / ((m_b-m_a).rot90()*(o.m_b-o.m_a));
    return true;
  }
  else
    return false;
}

bool Line::collide(const Line &o, V2D &cp)
{
  R r;
  if (collide(o, r))
  {
    cp = m_a+(m_b-m_a)*r;
    return true;
  }
  else
    return false;
}
