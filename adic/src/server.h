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
#include "commonconfig.h"
#include "metaserver.h"

//! server configuration
struct ServerConfig : public CommonConfig
{
  ServerConfig() : m_meshURI("data:adic.xml"), m_broadcastFreq(40)
  {}
  
  std::string m_meshURI;
  unsigned m_broadcastFreq;
  std::string m_myAddress;
};
DOPE_CLASS(ServerConfig);

template <typename Layer2>
inline void composite(Layer2 &layer2, ServerConfig &c)
{
  composite(layer2,static_cast<CommonConfig &>(c));
  layer2.simple(c.m_meshURI,"meshURI")
    .simple(c.m_broadcastFreq,"broadcastFreq")
    .simple(c.m_myAddress,"myAddress");
}

template <typename INP = InProto>
class StreamFactory
{
public:
  StreamFactory(std::streambuf &_l0) 
#if USE_RAW_PROTOCOL == 1
    : inp(_l0) , sigFactory(inp)
#elif USE_XML_PROTOCOL == 1
    : inp(_l0,TimeStamp(0,300),2) , sigFactory(inp)
#endif
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
  
  void handleGreeting(DOPE_SMARTPTR<ClientGreeting> gPtr);
  void handleInput(DOPE_SMARTPTR<Input> inputPtr);
  void handlePing(DOPE_SMARTPTR<Ping> pingPtr);
  void handleChatMessage(DOPE_SMARTPTR<ChatMessage> chatPtr);

  /*
  void handleFoo(DOPE_SMARTPTR<foo> fooPtr)
  {
    assert(fooPtr.get());
    ++msgs;
    std::cout << "saved foo from network to foo.net\n";
    writeFoo(*fooPtr.get(),"foo.net");
    }*/

  const std::vector<PlayerID> &getPlayerIDs() const
  {
    return playerIDs;
  }

  //! do everything needed for a game restart
  void restart()
  {
    playerIDs.clear();
  }
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
  //! client player IDs
  std::vector<PlayerID> playerIDs;

  //! this connection has none player
  bool noPlayer() const
  {
    return playerIDs.empty();
  }
  
  //! this connection has only one player
  bool singlePlayer() const
  {
    return playerIDs.size()==1;
  }
  //! this connection has only one team
  bool singleTeam() const;
};

//! the server application
class Server : public SigC::Object
{
protected:
  ServerConfig &m_config;
  Game m_game;
  typedef std::map<NetStreamBufServer::ID, DOPE_SMARTPTR<Connection> > Connections;
  Connections connections;
  //! emit will send the message to this client (m_emitFilter == m_allFilter => all clients)
  NetStreamBufServer::ID m_emitFilter;
  //! meta server secret or empty if metaserver is not used
  std::string m_msecret;
  //! address we report to metaSever
  Host m_maddr;

  static const NetStreamBufServer::ID m_allFilter=-1;
public:
  Server(ServerConfig &config);
  ~Server(){}

  //! get team
  /*!
    \param tname the teamname or if empty find a suitable team
    \return the team or NULL
  */
  Team *getTeam(const std::string &tname);

  //! get weakest team to add a player
  /*!
    \return Team or NULL
  */
  Team *getWeakestTeam();

  //! get frame number
  Frame getFrame() const
  {
    return m_game.getFrame();
  }
  
  void handleNewConnection(NetStreamBufServer::ID id, DOPE_SMARTPTR<NetStreamBuf> streamPtr)
  {
    std::cerr << "\nNew connection ("<<id<<")\n";
    assert(streamPtr.get());
    streamPtr->setTcpNoDelay(m_config.m_tcpNoDelay);
    m_emitFilter=id;
    Connection *c(new Connection(streamPtr,*this));
    connections[id]=DOPE_SMARTPTR<Connection>(c);
    broadcastNewClient(c);
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
    updateMetaserver();
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
  
  PlayerID addPlayer(const std::string &name, const std::string &URI)
  {
    return m_game.addPlayer(name,URI);
  }

  void setInput(const PlayerInput &i)
  {
    m_game.setInput(i);
  }

  //! broaadcast NewClient message
  /*!
    \note this message is broadcasted twice if a new client connects
  */
  void broadcastNewClient(Connection* c);

  //! send non-global chat message
  /*!
    sender must be set to either a player or team name
  */
  void sendChatMessage(ChatMessage &msg);

  std::vector<TeamID> getTeamIDs(const Connection *c) const;

  std::string getPlayerName(PlayerID pid) const
  {
    assert(pid<m_game.getPlayerNames().size());
    return m_game.getPlayerNames()[pid];
  }
  std::string getPlayersTeamName(PlayerID pid)
  {
    TeamID tid=m_game.getTeamIDofPlayer(pid);
    assert(tid!=TeamID(~0U));
    // this is not const => we are not const
    Team *t(m_game.getTeam(tid));
    assert(t);
    return t->name;
  }

protected:
  void restart();
  void addStartObjects();
  //! update metaserver status
  void updateMetaserver();
};

#endif
