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
   \file gui.h
   \brief The GUI base class
   \author Jens Thiele
*/

#ifndef GUI_H
#define GUI_H

#include "guiconfig.h"
#include "client.h"
#include "input.h"

class GUI : public SigC::Object
{
public:
  GUI(Client &client, const GUIConfig &config) : m_client(client), m_config(config)
  {}
  virtual ~GUI(){}
  
  virtual bool init()=0;
  virtual bool step(R dt)=0;
  //! get world position
  virtual V2D getPos() const=0;

  SigC::Signal1<void, Input &> input;
  SigC::Signal1<void, char> printed;
  SigC::Signal1<void, ChatMessage &> chatMessage;
  
  virtual std::ostream &getOstream()=0;

  virtual unsigned numInputDevices() const=0;
protected:
  Client &m_client;
  GUIConfig m_config;
};

class GUIFactory
{
public:
  GUI* create(Client &client, const GUIConfig &config);
};

#endif
