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
  //  streamPtr->setBlocking(false);
  factory.connect(SigC::slot(*this,&Connection::handleInput));
  factory.connect(SigC::slot(*this,&Connection::handleGreeting));
}

void 
Connection::handleGreeting(DOPE_SMARTPTR<ClientGreeting> gPtr)
{
  for (unsigned i=0;i<gPtr->m_userSetting.users.size();++i)
    {
      Team *t=server.getTeam(gPtr->m_userSetting.users[i].m_tname);
      // todo: the problem is that we send back an array
      // and the client doesn't know which players haven't been created
      if (!t) continue; 

      // todo - find unused player before adding a new one

      // check if team is full
      if (t->playerIDs.size()>=t->textures.size())
	continue;
      // try to add player
      assert(t->playerIDs.size()<t->dataURIs.size());
      PlayerID id=server.addPlayer(gPtr->m_userSetting.users[i].m_uname,t->dataURIs[t->playerIDs.size()]);
      if (id==PlayerID(~0U))
	break;
      playerIDs.push_back(id);
      t->addPlayer(id);
    }
  ServerGreeting g;
  g.m_players=playerIDs;
  emit(g);
  server.broadcastNewClient(this);
}


void 
Connection::handleInput(DOPE_SMARTPTR<Input> inputPtr)
{
  assert(inputPtr.get());
  // we have to remap the device no to the player no
  uint8_t devno=inputPtr->devno;
  if (devno>=playerIDs.size()) {
    DOPE_WARN("devno too big");
    return;
  }
  PlayerInput i(*inputPtr.get(),playerIDs[devno]);
  server.setInput(i);
  server.broadcast(i);
  //  std::cerr << "\nGot input signal\n";
}

Server::Server(ServerConfig &config) 
  : m_config(config), m_game(config.m_meshURI), m_quit(false), m_emitFilter(m_allFilter)
{
  Game::WorldPtr w(m_game.getWorldPtr());
  assert(w.get());
  const Mesh::StartObjects &objpos(w->getStartObjects());
  for (unsigned i=0;i<objpos.size();++i)
    m_game.addObject(objpos[i]);
}

Team *
Server::getTeam(const std::string &tname)
{
  Team *t=NULL;
  if (tname.empty()) {
    // no special team requested => find team which needs a player
    t=getWeakestTeam();
    if (!t) return NULL;
  }else{
    // does this team already exist ?
    t=m_game.getTeam(tname);
    if (t) {
      // yes 
      // => todo: password needed and correct ?
      // check if team is full
    }else{    
      // no => create new team 
      // todo: if maximum number of teams isn't reached yet
      t=m_game.addTeam(tname,m_game.numTeams());
    }
  }
  return t;
}

Team *
Server::getWeakestTeam()
{
  unsigned maxTeams=4;
  unsigned numTeams=m_game.numTeams();
  unsigned maxPlayers=4;
  if (numTeams<maxTeams)
    {
      // create team
      std::string tname("Team");
      tname+=anyToString(numTeams+1);
      return m_game.addTeam(tname,numTeams);
    }
  // todo: better alogrithm - check for max players
  unsigned minp=~0U;
  unsigned i=0;
  const std::vector<Team> &teams(m_game.getTeams());
  for (;i<numTeams;++i)
    minp=std::min(teams[i].playerIDs.size(),minp);
  if (minp>=maxPlayers)
    return NULL;
  for (i=0;i<numTeams;++i)
    if (teams[i].playerIDs.size()==minp)
      break;
  if (i>=numTeams)
    return NULL;
  return m_game.getTeam(i);
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
    std::cout << "\rUptime: " << std::fixed << std::setprecision(2) << std::setw(10) << uptime 
	      << " FPS: " << std::setw(8) << R(frames)/uptime 
	      << " Frame: " << std::setw(10) << frames;
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


void
Server::broadcastNewClient(Connection* c)
{
  NewClient m;
  m.playerNames=m_game.getPlayerNames();
  m.teams=m_game.getTeams();
  broadcast(m);
}

