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

#include "server.h"

void sigPipeHandler(int x){
  std::cerr << "WARNING: Received sig pipe signal - I ignore it"<<std::endl;
}

Connection::Connection(DOPE_SMARTPTR<NetStreamBuf> _streamPtr, Server &_server) 
  : streamPtr(_streamPtr), server(_server),
    factory(*(streamPtr.get())), outProto(*(streamPtr.get())), emitter(outProto)
{
  streamPtr->setBlocking(false);
  factory.connect(SigC::slot(*this,&Connection::handleInput));
  playerID=server.addPlayer();
  Greeting g(playerID);
  emit(g);
}

void 
Connection::handleInput(DOPE_SMARTPTR<Input> inputPtr)
{
  assert(inputPtr.get());
  PlayerInput i(*inputPtr.get(),playerID);
  server.setInput(i);
  server.broadcast(i);
  std::cerr << "\nGot input signal\n";
}

int 
Server::main()
{
  signal(SIGPIPE,sigPipeHandler);
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
  TimeStamp timeOut;
  oldTime.now();
  unsigned frames=0;
  while (!m_quit) {
    // todo - should it be a loop while(dataAvailable?)
    listener.select(&null); // test for input and emit signals if input is available
    newTime.now();
    dt=newTime-oldTime;
    while(dt<minStep) {
      timeOut=(minStep-dt);
      listener.select(&timeOut);
      newTime.now();
      dt=newTime-oldTime;
    }
    // main work
    m_game.step(R(dt.getSec())+R(dt.getUSec())/1000000);
    if (!(frames%100))
      broadcast(m_game);
    oldTime=newTime;
    ++frames;
    dt=newTime-start;
    R uptime=R(dt.getSec())+(R(dt.getUSec())/1000000);
    std::cout << "\rUptime: " << std::fixed << std::setprecision(2) << std::setw(15) << uptime 
	      << " FPS: " << std::setw(10) << R(frames)/uptime 
	      << " Frame: " << std::setw(20) << frames
	      << " Stamp: " << m_game.getTimeStamp().getSec() << "sec "<<m_game.getTimeStamp().getUSec() << "usec";
  }
  connections.clear();
  return 0;
}

int main(int argc,char *argv[])
{
  try{
    /* efence test
    char *foo=new char[99];
    //foo[100]=1; // above
    //    foo[-1]=2; // below
    delete [] foo;
    foo[10]=0; // free
    */

    ArgvParser parser(argc,argv);
    ServerConfig config;
    parser.simple(config,NULL);
    // exit if parser printed the help message
    if (parser.shouldExit()) return 1;
    Server server(config);
    return server.main();
  }
  catch (std::exception &error){
    std::cerr << "\nUncaught std::exception: "<<error.what()<<std::endl;
  }/*catch(...){
     std::cerr << "Uncaught unkown exception\n";
     }*/
  return 1;
}
