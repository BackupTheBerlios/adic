#include "line.h"

R Line::project(const V2D &p) const
{
  return (m_vn*(p-m_a));
}

R Line::dist(const V2D &p, V2D &vn)
{
  R r = project(p);
  R l = 0;

  if ((r>=0) && (r<=m_l))
    {
      l = (p-m_a)*m_n;
      if (l<0) 
	{
	  l *= -1;
	  vn = m_n*((R) -1); 
	}
      else vn = m_n;
    }
  else
    {
      V2D va = (p-m_a);
      V2D vb = (p-m_b);
      if (va.norm2() < vb.norm2()) vn = va; else vn = vb;
      l = vn.norm2();
      vn.normalize();
    }

  return l;
}

R Line::dist(const V2D &p)
{
  /*
  R r = project( p );
  return sqrt((p-m_a).norm2sqr()-r*r);*/
  V2D vn;
  return dist( p, vn );
}

bool Line::collide(const Line &o)
{
  R r;
  return collide(o, r);
}

bool Line::collide(const Line &o, R &cp)
{
  V2D ov = V2D(o.m_v);
  V2D ovn = V2D(o.m_vn);
  V2D oa = V2D(o.m_a);

  R t1 = m_v[0]*ovn[1]-m_v[1]*ovn[0];
  R t2 = ov[0]*m_vn[1]-ov[1]*m_vn[0];

  if (t1 == 0) return false;
  if (t2 == 0) return false;

  R n1 = ovn[0]*(oa[1]-m_a[1]) - ovn[1]*(oa[0]-m_a[0]);
  R n2 = m_vn[0]*(m_a[1]-oa[1]) - m_vn[1]*(m_a[0]-oa[0]);

  cp = -m_l*n1/t1;
  R cp2 = -o.m_l*n2/t2;
  
  if (((cp>=0) && (cp<=m_l)) && ((cp2>=0) && (cp2<=o.m_l))) return true; else return false;
}

bool Line::collide(const Line &o, V2D &cp)
{
  R r;
  if (collide(o, r))
  {
    cp = m_a+m_vn*r;
    return true;
  }
  else
    return false;
}




















