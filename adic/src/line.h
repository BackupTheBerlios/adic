#ifndef LINE_H
#define LINE_H

#include "vector2d.h"

//! line (vector) from a to b
class Line
{
public:
  Line()
  {
    m_a = V2D(0,0);
    m_b = V2D(1,1);
    m_v = V2D(m_b-m_a);
    m_l = m_v.norm2();
    m_vn = m_v/m_l;
    m_n = V2D(m_vn).rot90();
  }

  Line(const V2D &a, const V2D &b)
  {
    m_a=a;
    m_b=b;
    m_v = V2D(m_b-m_a);
    m_l = m_v.norm2();
    m_vn = m_v/m_l;
    m_n = V2D(m_vn);
    m_n.rot90();
  }
    
  V2D m_a,m_b,m_v,m_vn,m_n;
  R m_l;

  //! parallel projection of point p on Line orthogonal to line
  /*!
    find foot point (f)
    
    \returns parameter r in f=a+(b-a)*r; with (f-a)*(p-f)=0
  */
  R project(const V2D &p) const; // todo

  //! distance of point to line
  R dist(const V2D &p); // todo
  R dist(const V2D &p, V2D &vn);
  
  //! collide two line
  /*!
    \param cp collision point (only set if there is a collision - cross point)
    
    \returns true if the lines collide/cross otherwise false
  */
  bool collide(const Line &o, V2D &cp); // todo

  //! same as above but does not return cp
  bool collide(const Line &o); // todo

  //! same as above but does return the collision as parameter r in cp=a+(b-a)*r
  bool collide(const Line &o, R &cp); // todo

  //! translate line
  void translate(const V2D &tv)
  {
    m_a+=tv;
    m_b+=tv;
  }

  //! return "surface" normal
  V2D normal() const
  {
    return (m_b-m_a).rot90();
  }
};

#endif




