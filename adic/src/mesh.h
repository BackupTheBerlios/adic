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
   \file mesh.h
   \brief mesh
   \author Jens Thiele
*/

#ifndef MESH_H
#define MESH_H

#include "typedefs.h"
#include "vector2d.h"
#include <vector>


//! edge type
/*
  bit encoded edge type
*/
typedef uint8_t EdgeType;

#define DOORBIT 1


//! simple edge
/* this is a half edge
   
   this is especially important to note for the type flag
   this means you can specify two types for one full edge
   we could use this f.e. for doors which only open on one side
*/
struct Edge
{
  //! vertice ID type
  typedef int VID;
  //! end of edge loop marker
  static const VID noVertice=-1;
  
  //! vertice ID (start of edge)
  VID vid;
  //! type of edge 
  EdgeType type;
};
DOPE_CLASS(Edge);

template <typename Layer2>
inline void composite(Layer2 &layer2, Edge &e)
{
  layer2.simple(e.vid,"vid").simple(e.type,"type");
}


struct Object
{
  V2D pos;
  R dir;
  std::string URI;

  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(pos).SIMPLE(dir).SIMPLE(URI);
  }

};
DOPE_CLASS(Object);
template <typename Layer2>
inline void composite(Layer2 &layer2, Object &o)
{
  o.composite(layer2);
}


//! simple mesh as input to the world
class Mesh
{
public:
  //! edge ID type
  typedef int EID;
  typedef std::vector<std::pair<V2D,R> > StartPoints;
  typedef std::vector<Object> StartObjects;
  
  std::vector<V2D> vertices;
  std::vector<Edge> edgelist;

  StartPoints startPoints;
  StartObjects startObjects;
  
  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(vertices).SIMPLE(edgelist).SIMPLE(startPoints).SIMPLE(startObjects);
  }

  uint getNumFaces() const
  {
    uint r=0;
    for (std::vector<Edge>::const_iterator it=edgelist.begin();it!=edgelist.end();++it)
      {
	if (it->vid==Edge::noVertice)
	  ++r;
      }
    return r;
  }
  uint getNumEdges() const
  {
    return edgelist.size()-getNumFaces();
  }
  

  bool noEdge(EID e) const
  {
    return (e<0)||(e>=edgelist.size())||(edgelist[e].vid==Edge::noVertice);
  }
  bool isEdge(EID e) const 
  {
    return !noEdge(e);
  }
  bool lastEdgeOfRoom(EID e) const
  {
    return noEdge(++e);
  }
  
  EID getNCW(EID e) const
  {
    ++e;
    if (noEdge(e))
      {
	do {
	  --e;
	}while (isEdge(e));
	++e;
      }
    assert(isEdge(e));
    return e;
  }
};
DOPE_CLASS(Mesh);


template <typename Layer2>
inline void composite(Layer2 &layer2, Mesh &m)
{
  m.composite(layer2);
}


#endif
