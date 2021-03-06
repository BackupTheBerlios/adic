#include "polygon.h"

#include <algorithm> // std::max

Poly::Poly(const std::vector<V2D> &lineloop) 
  : m_lineloop(lineloop)
{
  m_xmax = 0;
  for(unsigned i=0; i<lineloop.size(); i++)
    {
      m_xmax = std::max( m_xmax, lineloop[i].m_v[0] );
    }
}

bool 
Poly::inside(const V2D &p) const
{
  if (m_lineloop.empty()) return false;
  
  int count = 0;

  Line l1, l2;
  V2D old(m_lineloop.back());
  //  std::cout << "xmax " << m_xmax << std::endl;
  for(unsigned i=0; i<m_lineloop.size(); i++)
    {
      l2 = Line(old, m_lineloop[i]);
      l1 = Line(p, V2D(m_xmax+10, p.m_v[1])); //max( l2.m_a[0], l2.m_b[0] );
      /*      std::cout << "poly i: " << i << std::endl;
      std::cout << "l1.m_a  " << l1.m_a << std::endl;
      std::cout << "l1.m_b  " << l1.m_b << std::endl;
      std::cout << "l2.m_a  " << l2.m_a << std::endl;
      std::cout << "l2.m_b  " << l2.m_b << std::endl;
      std::cout << "collide " << l1.collide(l2) << std::endl;*/
      if (l1.collide(l2)) count++;
      old = l2.m_b;
    }
  return count & 1;
}
