#include "wall.h"

bool
Wall::collide(const Circle &circle, V2D &cv) const
{
  R dist;
  return collide(circle, cv, dist);
}

bool
Wall::collide(const Circle &circle, V2D &cv, R &dist) const
{ 
  V2D dn;
  R dl = Line(m_l).dist(circle.m_pos, dn) - circle.m_r - m_wt;
  R dpa = (circle.m_pos-m_l.m_a).norm2() - circle.m_r - m_pr;
  R dpb = (circle.m_pos-m_l.m_b).norm2() - circle.m_r - m_pr;

  /*
  std::cout << "dl  " << dl << std::endl;
  std::cout << "dpa " << dpa << std::endl;
  std::cout << "dpb " << dpb << std::endl;
  std::cout << "m_wt" << m_wt << std::endl;
  std::cout << "m_pr" << m_pr << std::endl;
  std::cout << "c   " << circle.m_r << std::endl;
  */

  R d = 0;
  int c = 0;

  if (dl<0) 
    { c = 1; d = -dl; }

  if (dpa<0) 
    {
      if (c==0) { c = 2; d = -dpa; }
      else if (-dpa<d) { c = 2; d = -dpa; }
    }

  if (dpb<0)
    {
      if (c==0) { c = 3; d = -dpb; }
      else if (-dpb<d) { c = 3; d = -dpb; }
    }

  //  std::cout << "d " << d << std::endl;

  if (c==1)
    {
      cv = dn;
      dist = Line(m_l).project(circle.m_pos);
      //std::cout << "project (c=1): " << dist << std::endl;
      return true;
    }
  else if (c==2)
    {
      cv = (circle.m_pos-m_l.m_a);
      cv.normalize();
      dist = 0;
      return true;
    }
  else if (c==3)
    {
      cv = (circle.m_pos-m_l.m_b);
      cv.normalize();
      dist = m_l.m_l;
      return true;
    }
  else
    return false;
}
