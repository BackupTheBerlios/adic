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
   \file server.h<2>
   \brief the game server
   \author Jens Thiele
*/

#ifndef SERVER_H
#define SERVER_H

// layer 2 argument vector parser
#include <dope/argvparser.h>
#include <dope/dopesignal.h>
#include <dope/network.h>
// layer 2 mini xml output stream
#include <dope/minixml.h>
// layer 2 xml sax input
#include <dope/xmlsaxinstream.h>
#include <signal.h>

#include <iostream>
#include <exception>
#include "typedefs.h"
#include "game.h"
#include "messages.h"
#include "input.h"

//! server configuration
struct ServerConfig
{
  ServerConfig() : m_port(40700), m_meshURI("file:data/mesh.xml")
  {}
  
  unsigned short int m_port;
  std::string m_meshURI;
};
DOPE_CLASS(ServerConfig);

template <typename Layer2>
inline void composite(Layer2 &layer2, ServerConfig &c)
{
  layer2.simple(c.m_port,"port").simple(c.m_meshURI,"meshURI");
}

typedef XMLOutStream<std::streambuf> OutProto;
typedef XMLSAXInStream<std::streambuf> InProto;

template <typename INP = InProto>
class StreamFactory
{
public:
  StreamFactory(std::streambuf &_l0) : inp(_l0,TimeStamp(0,300),5) , sigFactory(inp)
  {
  }

  void read()
  {
    sigFactory.read();
  }
  
  template <typename X>
  void connect(const SigC::Slot1<void, DOPE_SMARTPTR<X> > &s)
  {
    sigFactory.connect(s);
  }
  
protected:
  INP inp;
  SignalInAdapter<INP> sigFactory;
};


class Server;

//! one class for each connection
/*! 
  \todo
  - why did I use StreamFactory which seems to wrap the SignalInAdapter only ?
*/
class Connection : public SigC::Object
{
public:
  Connection(DOPE_SMARTPTR<NetStreamBuf> _streamPtr, Server &_server);

  void read()
  {
    do {
      factory.read();
    }while(streamPtr->in_avail());
  }
  
  template <typename X>
  void emit(X &x)
  {
    emitter.emit(x);
  }
  
  void handleInput(DOPE_SMARTPTR<Input> inputPtr);
  
  /*
  void handleFoo(DOPE_SMARTPTR<foo> fooPtr)
  {
    assert(fooPtr.get());
    ++msgs;
    std::cout << "saved foo from network to foo.net\n";
    writeFoo(*fooPtr.get(),"foo.net");
    }*/

protected:
  //! pointer to out layer0 network stream
  DOPE_SMARTPTR<NetStreamBuf> streamPtr;

  Server &server;
  
  //! factory producing objects from out network stream
  StreamFactory<> factory;
  //! output protocol
  OutProto outProto;
  //! class to emit objects to the network stream
  SignalOutAdapter<OutProto> emitter;
  //! my player ID
  Game::PlayerID playerID;
};

//! the server application
class Server : public SigC::Object
{
protected:
  ServerConfig &m_config;
  Game m_game;
  typedef std::map<NetStreamBufServer::ID, DOPE_SMARTPTR<Connection> > Connections;
  Connections connections;
  //! should we quit ?
  bool m_quit;
  //! emit will send the message to this client (m_emitFilter == m_allFilter => all clients)
  NetStreamBufServer::ID m_emitFilter;

  static const NetStreamBufServer::ID m_allFilter=-1;
public:
  Server(ServerConfig &config) 
    : m_config(config), m_game(config.m_meshURI), m_quit(false), m_emitFilter(m_allFilter)
  {}
  ~Server(){}

  void handleNewConnection(NetStreamBufServer::ID id, DOPE_SMARTPTR<NetStreamBuf> streamPtr)
  {
    std::cerr << "\nNew connection ("<<id<<")\n";
    assert(streamPtr.get());
    m_emitFilter=id;
    connections[id]=DOPE_SMARTPTR<Connection>(new Connection(streamPtr,*this));
    emitGame();
  }

  void handleDataAvailable(NetStreamBufServer::ID id, DOPE_SMARTPTR<NetStreamBuf>)
  {
    assert(connections.find(id)!=connections.end());
    m_emitFilter=id;
    connections[id]->read();
  }

  void handleConnectionClosed(NetStreamBufServer::ID id, DOPE_SMARTPTR<NetStreamBuf>)
  {
    Connections::iterator it(connections.find(id));
    assert(it!=connections.end());
    std::cerr << "\nConnection ("<<id<<") closed\n";
    connections.erase(it);
  }

  int main();

  template <typename X>
  void emit(X &x) 
  {
    if (m_emitFilter==m_allFilter) {
      Connections::iterator it(connections.begin());
      while (it!=connections.end()) {
	it->second->emit(x);
	++it;
      }
    }else{
      Connections::iterator it(connections.find(m_emitFilter));
      DOPE_CHECK(it!=connections.end());
      it->second->emit(x);
    }
  }

  void emitGame() 
  {
    emit(m_game);
  }

  template <typename X>
  void broadcast(X &x) 
  {
    NetStreamBufServer::ID efbackup=m_emitFilter;
    m_emitFilter=m_allFilter;
    emit(x);
    m_emitFilter=efbackup;
  }
  
  Game::PlayerID addPlayer()
  {
    return m_game.addPlayer();
  }

  void setInput(const PlayerInput &i)
  {
    m_game.setInput(i);
  }
};

#endif