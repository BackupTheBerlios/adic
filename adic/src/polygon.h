#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include "vector2d.h"
#include "line.h"

//! class representing a polygon
class Polygon
{
protected:
  //! the polygon
  std::vector<V2D> m_lineloop;
  R m_xmax;
public:
  //! empty polygon
  Polygon()
  {}
  
  Polygon(const std::vector<V2D> &lineloop) : m_lineloop(lineloop)
  {
    m_xmax = 0;
    for(unsigned i=0; i<lineloop.size(); i++)
      {
	m_xmax = std::max( m_xmax, lineloop[i].m_v[0] );
      }
  }

  //! is the point inside of the polygon ?
  /*!
    \param p the point

    \returns true if point is inside
  */
  bool inside(const V2D &p) const;
};


#endif
