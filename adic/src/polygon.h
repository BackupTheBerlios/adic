#ifndef POLYGON_H
#define POLYGON_H

#include "vector2d.h"
#include "line.h"
#include <vector>

//! class representing a polygon
/*!
  \note renamed it from Polygon to Poly because of a name clash with the WINAPI
*/
class Poly
{
protected:
  //! the polygon
  std::vector<V2D> m_lineloop;
  R m_xmax;
public:
  //! empty polygon
  Poly()
  {}
  
  Poly(const std::vector<V2D> &lineloop);

  //! is the point inside of the polygon ?
  /*!
    \param p the point

    \returns true if point is inside
  */
  bool inside(const V2D &p) const;

  const std::vector<V2D> &getLineLoop() const
  {
    return m_lineloop;
  }
};


#endif
