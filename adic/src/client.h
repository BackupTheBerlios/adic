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
   \file client.cpp
   \brief client main
   \author Jens Thiele
*/

#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <exception>
// layer 2 argument vector parser
#include <dope/argvparser.h>
#include <dope/network.h>
#include <dope/dopesignal.h>

#include <signal.h>

#include "typedefs.h"
#include "game.h"
#include "messages.h"
#include "guiconfig.h"

struct ClientConfig
{
  ClientConfig() : m_port(40700), m_server("localhost")
  {}
  
  unsigned short int m_port;
  std::string m_server;
  GUIConfig m_gui;
};
DOPE_CLASS(ClientConfig);
template <typename Layer2>
inline void composite(Layer2 &layer2, ClientConfig &c)
{
  layer2.simple(c.m_port,"port").simple(c.m_server,"server").simple(c.m_gui,"gui");
}

typedef XMLOutStream<std::streambuf> OutProto;
typedef XMLSAXInStream<std::streambuf> InProto;

class Client : public SigC::Object
{
protected:
  ClientConfig &m_config;
  Game m_game;
  bool m_quit;
public:
  Client(ClientConfig &config) : m_config(config), m_quit(false)
  {}
  ~Client(){}

  void handleGreeting(DOPE_SMARTPTR<Greeting> gPtr)
  {
    assert(gPtr.get());
    std::cerr << "\nGot Greeting from server "<< gPtr->m_adicVersion.asString() << " DOPE++ "<<gPtr->m_dopeVersion.asString()<<"\n";
  }
  
  void handleGame(DOPE_SMARTPTR<Game> gPtr)
  {
    assert(gPtr.get());
    std::cerr << "\nGot game data\n";
    // todo later we will perhaps need a replace method
    m_game=*gPtr.get();
  }
  
  int main();

  Game::WorldPtr getWorldPtr()
  {
    return m_game.getWorldPtr();
  }
protected:
};

#endif
