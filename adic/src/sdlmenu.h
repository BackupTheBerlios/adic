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
   \author Jens Schwarz, Jens Thiele
*/

#ifndef SDLMENU_H
#define SDLMENU_H

#include "messages.h"
#include "team.h"
#include "clientconfig.h"
#include "sdlglgui.h"

//! menu displayed on game start
/*!
  the job of the menu ist to fill the GUIClientConfig config member
  and to emit signals reporting which fields have changed

  you can select: (corresponding member)
  the servername (config.m_server)
  the number of players (the size of config.m_users.users)
  for each player:
  - name (config.m_users.users.m_uname)
  - team name (config.m_users.users.m_tname)

  save configuration (save.emit())
*/
class SDLMenu : public SigC::Object
{
public:
  SDLMenu(SDLGLGUI &_gui);
  ~SDLMenu(){}

  //! repaint
  /*
    \return true if still alive - return false to remove this menu
  */
  bool step(R dt);


  // signal handlers (input)

  //! is called when input device has changed status
  void handleInput(Input &i);

  //! is called when a new client connected
  /*!
    see messages.h for the NewClient struct
   */
  void handleNewClient(DOPE_SMARTPTR<NewClient> mPtr);

  // signal output

  //! signal is emitted if user selected a server
  /*!
    \return true on success otherwise false
  */
  SigC::Signal0<bool> serverSelected;

  //! signal is emitted if user finished input
  SigC::Signal0<void> configured;

  //! emitted when a key is pressed / this could be used to play a sound
  /*!
    the char is the key that was pressed. Different keys might have different sounds
    f.e. '\n' should have a completely different sound from 'A'
  */
  SigC::Signal1<void, char> printed;

  //! emitted if the configuration should be saved now
  SigC::Signal0<void> save;

protected:
  // internally used:

  void handleServerName(const std::string &name);
  void handleServerPort(const std::string &name);
  void handlePlayerName(const std::string &name);
  void handleTeamName(const std::string &name);




  //! this is called when the menu is finished
  /*!
    this is mainly to demonstrate Jens Schwarz how to emit signals ;-)
  */
  void finish() 
  {
    configured.emit();
  }
  
  SDLGLGUI &gui;
  SDLGL &gl;
  //! the whole job of SDLMenu is to fill this struct
  GUIClientConfig &config;
  
  SDLInputField serverName;
  SDLInputField serverPort;
  SDLInputField playerName;
  SDLInputField teamName;

  int m_screen, m_playerNum;
  DOPE_SMARTPTR<Texture> m_hiPtr;

  std::vector<DOPE_SMARTPTR<Texture> > m_screens;

};

#endif
