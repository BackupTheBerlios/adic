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
   \file team.h
   \brief class Team
   \author Jens Thiele
*/

#ifndef TEAM_H
#define TEAM_H

#include "typedefs.h"
#include <string>
#include <dope/typenames.h>

struct TeamStat
{
  unsigned locked;
  unsigned numPlayers;

  bool allLocked() const
  {
    return locked==numPlayers;
  }
};

struct Team
{
  Team() {}
  //! create team with default values
  Team(const std::string &_name,unsigned tno);
  
  std::string name;
  float color[3];
  std::vector<std::string> dataURIs;
  std::vector<std::vector<std::string> > textures;
  std::vector<PlayerID> playerIDs;
  bool operator==(const std::string &n) const
  {
    return name==n;
  }
  void addPlayer(PlayerID id)
  {
    playerIDs.push_back(id);
  }
};
DOPE_CLASS(Team);
template <typename Layer2>
inline void composite(Layer2 &layer2, Team &t)
{
  layer2.simple(t.name,"name").simple(t.playerIDs,"playerIDs")
    .fixedCVector(t.color,3,"color")
    .simple(t.textures,"textures").simple(t.dataURIs,"dataURIs");
}

#endif
