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
   \file server.cpp
   \brief server main
   \author Jens Thiele
*/

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

void sigPipeHandler(int x){
  std::cerr << "WARNING: Received sig pipe signal - I ignore it"<<std::endl;
}

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

template <typename P = InProto >
class StreamFactory
{
public:
  StreamFactory(std::streambuf &_l0) : p(_l0) , sigFactory(p)
  {
  }

  void read()
  {
    // todo: only one object is read but perhaps there is more than one
    // does it at least get read in the next loop ?
    sigFactory.read();
  }
  
  template <typename X>
  void connect(const SigC::Slot1<void, DOPE_SMARTPTR<X> > &s)
  {
    sigFactory.connect(s);
  }
  
protected:
  P p;
  SignalInAdapter<P> sigFactory;
};

//! one class for each connection
/*! 
  \todo
  - why did I use StreamFactory which seems to wrap the SignalInAdapter only ?
*/
class Connection : public SigC::Object
{
public:
  Connection(DOPE_SMARTPTR<NetStreamBuf> _streamPtr) : streamPtr(_streamPtr), factory(*(streamPtr.get()))
  {
    //    factory.connect(SigC::slot(*this,&Connection::handleFoo));
  }
  
  void read()
  {
    factory.read();
  }

  /*
  void handleFoo(DOPE_SMARTPTR<foo> fooPtr)
  {
    assert(fooPtr.get());
    ++msgs;
    std::cout << "saved foo from network to foo.net\n";
    writeFoo(*fooPtr.get(),"foo.net");
    }*/

protected:
  DOPE_SMARTPTR<NetStreamBuf> streamPtr;
  StreamFactory<> factory;
};

//! the server application
class Server : public SigC::Object
{
protected:
  ServerConfig &m_config;
  Game m_game;
  typedef std::map<NetStreamBufServer::ID, DOPE_SMARTPTR<Connection> > Connections;
  Connections connections;
  bool m_quit;
  
public:
  Server(ServerConfig &config) : m_config(config), m_game(config.m_meshURI), m_quit(false)
  {}
  ~Server(){}

  void handleNewConnection(NetStreamBufServer::ID id, DOPE_SMARTPTR<NetStreamBuf> streamPtr)
  {
    std::cerr << "New connection ("<<id<<"\n";
    assert(streamPtr.get());
    connections[id]=DOPE_SMARTPTR<Connection>(new Connection(streamPtr));
  }

  void handleDataAvailable(NetStreamBufServer::ID id, DOPE_SMARTPTR<NetStreamBuf>)
  {
    assert(connections.find(id)!=connections.end());
    connections[id]->read();
  }

  void handleConnectionClosed(NetStreamBufServer::ID id, DOPE_SMARTPTR<NetStreamBuf>)
  {
    Connections::iterator it(connections.find(id));
    assert(it!=connections.end());
    std::cout << "Connection "<<id<<" closed\n";
    connections.erase(it);
  }

  int main()
  {
    NetStreamBufServer listener(m_config.m_port);
    listener.init();
    listener.newConnection.connect(SigC::slot(*this,&Server::handleNewConnection));
    listener.dataAvailable.connect(SigC::slot(*this,&Server::handleDataAvailable));
    listener.connectionClosed.connect(SigC::slot(*this,&Server::handleConnectionClosed));
    TimeStamp start;
    start.now();
    TimeStamp oldTime;
    TimeStamp newTime;
    TimeStamp minStep(0,1000000/100); // max. 100 Hz will be less because of the min sleep problem
    TimeStamp dt;
    TimeStamp null;
    oldTime.now();
    unsigned frames=0;
    while (!m_quit) {
      // todo - should it be a loop while(dataAvailable?)
      listener.select(&null); // test for input and emit signals if input is available
      newTime.now();
      dt=newTime-oldTime;
      while(dt<minStep) {
	(minStep-dt).sleep();
	newTime.now();
	dt=newTime-oldTime;
      }
      m_game.step(R(dt.getSec())+R(dt.getUSec())/1000000);
      oldTime=newTime;
      ++frames;
      dt=newTime-start;
      R uptime=R(dt.getSec())+(R(dt.getUSec())/1000000);
      std::cout << "\rUptime: " << std::fixed << std::setprecision(2) << std::setw(15) << uptime << " FPS: " << std::setw(10) << R(frames)/uptime << " Frame: " << std::setw(20) << frames;
    }
    connections.clear();
    return 0;
  }
};


int main(int argc,char *argv[])
{
  try{
    signal(SIGPIPE,sigPipeHandler);
    ArgvParser parser(argc,argv);
    ServerConfig config;
    parser.simple(config,NULL);
    // exit if parser printed the help message
    if (parser.shouldExit()) return 1;
    Server server(config);
    return server.main();
  }
  catch (std::exception &error){
    std::cerr << "Uncaught std::exception: "<<error.what()<<std::endl;
  }/*catch(...){
    std::cerr << "Uncaught unkown exception\n";
    }*/
  return 1;
}
