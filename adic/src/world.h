/*
 * Copyright (C) 2002 Jens Thiele <jens.thiele@student.uni-tuebingen.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*!
   \file world.h
   \brief The game world
   \author Jens Thiele
*/

#ifndef WORLD_H
#define WORLD_H

#include "mesh.h"
#include "circle.h"
#include "wall.h"

//! full winged edge
/*!
  this is the information about one wall and alll its direct neighbours
*/
struct FWEdge
{
  FWEdge() 
    : m_ncw(noEdge), m_pcw(noEdge), m_nccw(noEdge), m_pccw(noEdge), m_rcw(noRoom), m_rccw(noRoom),
      m_etcw(0), m_etccw(0)
  {
  }
  
  typedef unsigned VID;
  typedef unsigned EID;
  typedef unsigned RoomID;
  
  static const EID noEdge=~0U;
  static const RoomID noRoom=~0U;

  //! start vertice
  VID m_sv;
  //! end vertice
  VID m_ev;

  //! next edge clock wise
  EID m_ncw;
  //! previous edge clock wise
  EID m_pcw;
  //! next edge counter clock wise
  EID m_nccw;
  //! previous edge counter clock wise
  EID m_pccw;
  
  //! room clock wise
  RoomID m_rcw;
  //! room counter clock wise
  RoomID m_rccw;

  //! edge type clock wise
  EdgeType m_etcw;
  //! edge type counter clock wise
  EdgeType m_etccw;

  //! is this edge a door ?
  bool isDoor() const
  {
    return (m_etcw&DOORBIT)||(m_etccw&DOORBIT);
  }

  //! the corresponding wall
  Wall m_wall;
};

//! room
/*!
  you can think of it as a face in mesh
*/
struct Room
{
  //! one of our full winged edges
  FWEdge::EID m_e;
  //! is this edge clockwise
  bool m_cw;
};


//! the game world space
/*! 
  2D-World with walls and pillars and doors - see also the figure in the concept 

  the primary data structure is a BREP - in full winged edge representation
*/
class World
{
protected:
  //! all our vertices
  std::vector<V2D> m_vertices;
  //! full winged edge list
  std::vector<FWEdge> m_edges;
  //! room / face list
  std::vector<Room> m_rooms;
  //! all our start points
  std::vector<V2D> m_startPoints;
  //! temporary mesh object
  DOPE_SMARTPTR<Mesh> m_meshptr;
  //! stack depth counter for composite
  int m_stackDepth;

  //! common to call constructors
  void init()
  {
    m_stackDepth=0;
  }
  
public:
  // empty world
  World() {init();}

  //! create world from mesh
  World(const Mesh &mesh);
  
  //! in which room is this point ?
  /*!
    \param p the point to check

    \returns the ID of the room or FWEdge::noRoom
  */
  FWEdge::RoomID inRoom(const V2D &p);

  //! is the point in this room ?
  /*!
    \param p the point to check
    \param r the ID of the room to check
    
    \returns true if the point is in the room otherwise false
  */
  bool isInRoom(const V2D &p, FWEdge::RoomID r);
  
  //! collide circle with one room
  /*!
    \param c the circle to collide
    \param room the room to collide the circle with
    \param collision vector is only set if there is a collision

    \return true if there is a collision otherwise false

    \note doors are not collided ! this has to done in an extra step
  */
  bool collide(const Circle &c, FWEdge::RoomID room, V2D &cv) const;

  unsigned getNumVertices() const
  {
    return m_vertices.size();
  }
  unsigned getNumWalls() const
  {
    return m_edges.size();
  }
  unsigned getNumRooms() const
  {
    return m_rooms.size();
  }
  const std::vector<V2D> &getStartPoints() const
  {
    return m_startPoints;
  }
  
  //! return lineloop for one room
  /*!
    \room the ID of the room
  */
  std::vector<V2D> getLineLoop(FWEdge::RoomID room) const;

  //! return all doors
  std::vector<FWEdge::EID> getAllDoors() const;
  
  //! get edge
  const FWEdge &getEdge(FWEdge::EID eid) const
  {
    DOPE_CHECK(eid<m_edges.size());
    return m_edges[eid];
  }
  //! get start point of edge
  const V2D &getPoint(FWEdge::VID v) const
  {
    DOPE_CHECK(v<m_vertices.size());
    return m_vertices[v];
  }
  
  
  void setFromMesh(const Mesh &mesh);
  
  /*
    template <typename Layer2>
    void composite(Layer2 &layer2, True)
    {

    // BUG: see also <dope/xmlsaxinstream.h>
    Mesh m;
    layer2.simple(m,NULL);
    setFromMesh(m);
    // here the mesh will be destroyed and we will loose its data 
    // => this is (in the moment) not allowed with the XMLSAXInStream

    this does not work either: 
    i thought a workaround would be to count the stack depth - but it isn't since
    the xmlsaxinstream returns completely and just calls us more than once ?
    is this true ?
    }
  */


  bool getWall(FWEdge::EID eid, Wall &r) const
  {
    const FWEdge &e=getEdge(eid);
    if (e.isDoor())
      return false;
    r=e.m_wall;
    return true;
    /*    
    DOPE_CHECK(e.m_sv<m_vertices.size()&&e.m_ev<m_vertices.size());
    Line l(m_vertices[e.m_sv],m_vertices[e.m_ev]);
    r=Wall(l);
    return true;
    */
  }
  
  //! helper to walk through a room along the walls
  /*!
    this iterator will not loop
  */
  class EdgeIterator
  {
  protected:
    //! the world we are operating on
    const World &m_w;
    //! current edge is clock wise
    bool m_cw;
    //! this iterator is at the end
    bool m_end;
    //! current edge ID
    FWEdge::EID m_c;
    //! edge ID at beginning
    FWEdge::EID m_b;
  public:
    //! iterator which is already at the end
    EdgeIterator(const World &world) : m_w(world), m_end(true), m_c(FWEdge::noEdge), m_b(FWEdge::noEdge)
    {
    }
    
    //! iterator starting with first edge of a room
    EdgeIterator(const World &world, FWEdge::RoomID room);

    EdgeIterator &operator++()
    {
      return inc();
    }
    
    //! go to next wall
    EdgeIterator &inc();

    bool operator!=(const EdgeIterator &o) const
    {
      if ((m_end)&&(o.m_end)) return false;
      return (m_c!=o.m_c)||(m_b!=o.m_b); // todo is this useful ?
    }
    const FWEdge& operator->() const
    {
      return getEdge();
    }
    const FWEdge& operator*() const
    {
      return getEdge();
    }
    const FWEdge& getEdge() const
    {
      assert(m_c<m_w.m_edges.size());
      return m_w.m_edges[m_c];
    }
    //! get start point of this edge
    const V2D &getStartPoint() const
    {
      FWEdge::VID id;
      if (m_cw) id=getEdge().m_sv;
      else id=getEdge().m_ev;
      assert(id<m_w.m_vertices.size());
      return m_w.m_vertices[id];
    }
    //! get end point of this edge
    const V2D &getEndPoint() const
    {
      FWEdge::VID id;
      if (!m_cw) id=getEdge().m_sv;
      else id=getEdge().m_ev;
      assert(id<m_w.m_vertices.size());
      return m_w.m_vertices[id];
    }
    //! get corresponding wall
    /*!
      \param r the result
      \return true if a wall has been stored in r otherwise false
    */
    bool getWall(Wall &r) const
    {
      return m_w.getWall(m_c,r);
    }
    FWEdge::EID getID() const
    {
      return m_c;
    }
  };
};

/*
DOPE_CLASS(World);
template <typename Layer2>
inline void composite(Layer2 &layer2, World &w)
{
  w.composite(layer2,typename Layer2::Traits::InStream());
}
*/

#endif
