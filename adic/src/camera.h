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
  \file camera.h
  \brief class Camera
  \author Jens Thiele
*/

#ifndef CAMERA_H
#define CAMERA_H

#include "vector2d.h"
//! camera
/*!
  You can set position, zoom and rotation to a wished position
  and the camera will smoothly try to reach them
*/
class Camera
{
public:
  Camera();
  Camera(const V2D &cpos, const V2D &wpos,
	 R czoom, R wzoom, 
	 R crotate, R wrotate);

  ~Camera();

  const V2D& getPos() const
  {
    return m_cPos;
  }
  const V2D& getWPos() const
  {
    return m_wPos;
  }
  R getZoom() const
  {
    return m_cZoom;
  }
  R getWZoom() const
  {
    return m_wZoom;
  }
  R getRotate() const
  {
    return m_cRotate;
  }
  R getWRotate() const
  {
    return m_wRotate;
  }
  void step(R dt);

  void setPos(const V2D &w)
  {
    m_wPos=w;
  }
  void setZoom(R w)
  {
    m_wZoom=w;
  }
  void setRotate(R w)
  {
    m_wRotate=w;
  }
  void setMaxZoomSpeed(R m)
  {
    m_mzSpeed=m;
  }
protected:
  //! wished position
  V2D m_wPos;
  //! current position
  V2D m_cPos;
  //! current speed
  V2D m_cpSpeed;
  //! max speed
  R   m_mpSpeed;
  
  //! wished zoom
  R m_wZoom;
  //! current zoom
  R m_cZoom;
  //! current zoom speed
  R m_czSpeed;
  //! max zoom speed
  R m_mzSpeed;
  
  //! wished rotation
  R m_wRotate;
  //! current rotation
  R m_cRotate;
  //! current rotational speed
  R m_crSpeed;
  //! max rotational speed
  R m_mrSpeed;

  //! age of camera
  R m_age;
};

#endif
