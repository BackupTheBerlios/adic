/*
 * Copyright (C) 2002 Jens Thiele <karme@berlios.de>
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
   \file player.h
   \brief A player in the game world
   \author Jens Thiele
*/

#ifndef PLAYER_H
#define PLAYER_H

#include "roundobject.h"
#include "playerdata.h"
#include "genericpointer.h"
#include "uriloader.h"

//! a player (or object similar to a player) in the game world
/*!
  \note there are also non-player objects represented by this class
 */
class Player : public RoundObject
{
public:
  Player();
  Player(const V2D &pos, R dir, const std::string &playerDataURI);

  typedef GenericPointer<PlayerData, std::string, URILoader<URICache<PlayerData> > > PlayerDataPtr;
  
  bool step(R dt);

  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    RoundObject::composite(layer2);
    layer2.SIMPLE(m_speed).SIMPLE(m_direction).SIMPLE(m_ix).SIMPLE(m_iy).SIMPLE(m_playerDataPtr)
      .SIMPLE(m_fitness);// .SIMPLE(m_oldSpeed).SIMPLE(m_oldDirection);
  }

  void setControl(int8_t x, int8_t y)
  {
    m_ix=x;
    m_iy=y;
  }

  //! increase fitness
  /*!
    \param f increase to f*maxfitness
  */
  void increaseFitness(R f=1.0);

  R getFitness() const
  {
    return m_fitness;
  }
  
  R getDirection() const
  {
    return m_direction;
  }

  void rollback() 
  {
    RoundObject::rollback();
    /*
      m_speed=m_oldSpeed;
      m_direction=m_oldDirection;
      DOPE_ASSERT(m_speed==m_oldSpeed);
      DOPE_ASSERT(m_direction==m_oldDirection);
    */
  }
  void commit()
  {
    setOldValues(*this);
  }

  void setOldValues(Player &p)
  {
    RoundObject::setOldValues(p);
  }
  

  const V2D& getSpeed() const
  {
    return m_speed;
  }

  V2D getImpuls() const
  {
    DOPE_ASSERT(m_playerDataPtr.get());
    return getSpeed()*m_playerDataPtr->mass;
  }
  
  void applyImpuls(const V2D &i)
  {
    DOPE_ASSERT(m_playerDataPtr.get());
    m_speed+=i/m_playerDataPtr->mass;
  }

  bool isPlayer() const
  {
    DOPE_ASSERT(m_playerDataPtr.get());
    return m_playerDataPtr->type<10;
  }
  int8_t getType() const
  {
    DOPE_ASSERT(m_playerDataPtr.get());
    return m_playerDataPtr->type;
  }
  void setLocked(bool _locked)
  {
    locked=_locked;
  }
  bool isLocked() const
  {
    return locked;
  }

  int8_t getX() const
  {
    return locked ? 0 : m_ix;
  }
  int8_t getY() const
  {
    return locked ? 0 : m_iy;
  }
protected:

  //! our crrent speed vector
  V2D m_speed;
  //! our direction we are heading
  R m_direction;
  //! input x
  int8_t m_ix;
  //! input y
  int8_t m_iy;
  //! data that isn't changed
  PlayerDataPtr m_playerDataPtr;
  //! 0<fit<1
  R m_fitness;
  
  /*
    V2D m_oldSpeed;
    R m_oldDirection;
  */
  bool locked;

public:
  //! player data is relative to a default
  static PlayerDataPtr m_defaultDataPtr;
};
DOPE_CLASS(Player);

template <typename Layer2>
inline void composite(Layer2 &layer2, Player &p)
{
  p.composite(layer2);
}

#endif
