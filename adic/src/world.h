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

//! full winged edge
/*!
  this is the information about one wall and alll its direct neighbours
*/
struct FWEdge
{
  FWEdge() 
    : m_ncw(noEdge), m_pcw(noEdge), m_nccw(noEdge), m_pccw(noEdge), m_rcw(noRoom), m_rccw(noRoom)
  {
  }
  
  typedef int VID;
  typedef int EID;
  typedef int RoomID;
  
  static const EID noEdge=-1;
  static const RoomID noRoom=-1;

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

public:
  // empty world
  World()
  {}

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

  int getNumVertices() const
  {
    return m_vertices.size();
  }
  int getNumWalls() const
  {
    return m_edges.size();
  }
  int getNumRooms() const
  {
    return m_rooms.size();
  }

  void setFromMesh(const Mesh &mesh);
  
  template <typename Layer2>
  void composite(Layer2 &layer2, True)
  {
    Mesh m;
    layer2.simple(m,NULL);
    setFromMesh(m);
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
    EdgeIterator(const World &world) : m_w(world), m_end(true)
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
  };
};
DOPE_CLASS(World);

template <typename Layer2>
inline void composite(Layer2 &layer2, World &w)
{
  w.composite(layer2,typename Layer2::Traits::InStream());
}


#endif
