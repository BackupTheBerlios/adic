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
   \file sdlmenu.h
   \brief class SDLMenu
   \author Jens Thiele
*/

#ifndef SDLMENU_H
#define SDLMENU_H

#include "sdlglgui.h"
#include "messages.h"
#include "team.h"
#include "clientconfig.h"

//! menu displayed on game start
/*!
  you can select:
  the servername
  the number of players
  for each player:
  - name
  - team name

  save configuration
*/

class SDLMenu : public SigC::Object
{
public:
  SDLMenu(SDLGLGUI &_gui, const ClientConfig &_config)
    : gui(_gui), gl(_gui.gl), config(_config)
  {}
  ~SDLMenu(){}

  //! repaint
  /*
    \return true if still alive - return false to remove this menu
  */
  bool step(R dt);


  // signal handlers (input)

  //! is called when input device has changed status
  void handleInput(Input &i);

  //! is called when server connection succeeded or failed
  void handleServerConnect(bool success);

  //! is called when we received new server statistics
  /*!
    \todo this is not perfect because the menu has to assume
    things like max player/team and max team which it should not
  */
  void handleServerStat(const std::vector<TeamStat> &stats);
  
  // signal output

  //! signal is emitted if user selected a server
  SigC::Signal1<void, const std::string &> serverSelected;

  //! signal is emitted if user finished input
  SigC::Signal1<void, const ClientConfig &> configured;

  //! emitted when a key is pressed / this could be used to play a sound
  /*!
    the char is the key that was pressed. Different keys might have different sounds
    f.e. '\n' should have a completely different sound from 'A'
  */
  SigC::Signal1<void, char> printed;

  //! emitted if the configuration should be saved now
  SigC::Signal1<void, const ClientConfig &> save;
protected:
  //! this is called when the menu is finished
  /*!
    this is mainly to demonstrate Jens Schwarz how to emit signals ;-)
  */
  void finish() 
  {
    configured.emit(config);
  }
  
  SDLGLGUI &gui;
  SDLGL &gl;
  ClientConfig config;
};

#endif
