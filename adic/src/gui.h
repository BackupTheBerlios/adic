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
   \file gui.h
   \brief The GUI base class
   \author Jens Thiele
*/

#ifndef GUI_H
#define GUI_H

#include "guiconfig.h"
#include "client.h"
#include "input.h"

//! interface for different client guis
/*!
  \todo this is in a really bad shape. A lot of code currently is in the SDLGLGUI
  class which is the only implementation of this interface for now. This will improve
  with an alternate implementation that will need a lot of code from SDLGLGUI.
*/
class GUI : public SigC::Object
{
public:
  GUI(Client &client) : m_client(client)
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

  virtual void handleNewClient(DOPE_SMARTPTR<NewClient> mPtr)=0;
  virtual void handleGreeting(DOPE_SMARTPTR<ServerGreeting> gPtr)=0;
  virtual void handleEndGame(DOPE_SMARTPTR<EndGame> egPtr)=0;
  
  GUIConfig &getGUIConfig()
  {
    return m_client.getConfig().m_gui;
  }
  GUIClientConfig &getConfig()
  {
    return m_client.getConfig();
  }
protected:
  Client &m_client;
};

//! gui factory class
/*!
  creates a gui depending on the client configuration
*/
class GUIFactory
{
public:
  GUI* create(Client &client);
};

#endif
