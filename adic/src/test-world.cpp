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
   \file test-world.cpp
   \brief Test the world class
   \author Jens Thiele
*/

#include "world.h"

#include <fstream>

int main(int argc, char** argv)
{
  Mesh m;
  const char *fname="data/mesh.xml";
  if (argc>=2) fname=argv[1];
  
  std::ifstream inf(fname);
  if (!inf.good()) {
    std::cerr << "Could not find data file: \""<<fname<<"\"\n";
    return 1;
  }
  ConfigInStream is(*inf.rdbuf());
  is.simple(m,NULL);
  std::cout << "Vertices: " << m.vertices.size() << "\n";
  std::cout << "Half-Edges: " << m.getNumEdges() << "\n";
  std::cout << "Faces: " << m.getNumFaces() << "\n";
  World w(m);
  std::cout << "Vertices: " << w.getNumVertices() << "\n";
  std::cout << "Walls: " << w.getNumWalls() << "\n";
  std::cout << "Rooms: " << w.getNumRooms() << "\n";
}
