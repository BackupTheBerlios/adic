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
   \file client.cpp
   \brief client main
   \author Jens Thiele
*/

#include "botclient.h"
#include "bot.h"
#include <dope/dopestl.h>
#include <dope/argvparser.h>
#include <dope/dopesignal.h>
#include <signal.h>

void sigPipeHandler(int x){
  std::cerr << "\nWARNING: Received sig pipe signal - I ignore it\n"<<std::endl;
}


BotClient::BotClient(BotClientConfig &config) 
  : m_config(config), m_quit(false)
{}

BotClient::~BotClient() 
{
}

void
BotClient::handleGreeting(DOPE_SMARTPTR<ServerGreeting> gPtr)
{
  assert(gPtr.get());
  std::cout << "\nGot Greeting from server.\nServer is running ver. "<< gPtr->m_adicVersion.asString() << "\n";
  std::cout << "(DOPE++ ver. "<<gPtr->m_dopeVersion.asString()<<")\n";
  m_playerIDs=gPtr->m_players;

  unsigned got=m_playerIDs.size();
  unsigned req=m_config.m_users.users.size();
  std::cout << "I Got "<< got <<" player IDs (requested "<< req <<" )\n";
  if ((req>0)&&(!got))
    std::cout << "The server is full.\n";

  BotFactory factory;
  m_bots.clear();
  const std::vector<PlayerID> &pids(getMyIDs());
  for (unsigned p=0;p<pids.size();++p){
    m_bots.push_back(DOPE_SMARTPTR<Bot>(factory.create(*this,pids[p],p)));
    m_bots[p]->input.connect(SigC::slot(m_streamPtr->so,&SignalOutAdapter<OutProto>::emit<Input>));
    m_bots[p]->chatMessage.connect(SigC::slot(m_streamPtr->so,&SignalOutAdapter<OutProto>::emit<ChatMessage>));
  }
}

void 
BotClient::handleGame(DOPE_SMARTPTR<Game> gPtr)
{
  assert(gPtr.get());
  m_game.replace(*gPtr);
  //  reconnect signals
  m_game.collision.connect(SigC::slot(*this,&BotClient::handleCollision));
}

void 
BotClient::handleCollision(V2D pos, R strength)
{
}

void
BotClient::handlePlayerInput(DOPE_SMARTPTR<PlayerInput> iPtr)
{
  assert(iPtr.get());
  if (m_config.m_lagCompensation) {
    //    std::cerr << "\nInput Lag: "<<(m_game.getFrame()-iPtr->frame)<<" frames\n";
    if (m_game.getFrame()-iPtr->frame>=0)
      m_game.setInput(*iPtr.get());
    else{
      //      std::cerr << "Stored input in queue\n";
      m_inputQueue.push_back(iPtr);
    }
  }else{
    m_game.setInput(*iPtr.get());
  }
}

void
BotClient::handleNewClient(DOPE_SMARTPTR<NewClient> mPtr)
{
  m_game.setPlayerNames(mPtr->playerNames);
  m_game.setTeams(mPtr->teams);
}

std::string
BotClient::getPlayerName(PlayerID id) const
{
  if (id>=m_game.getPlayerNames().size()) return anyToString(id);
  //else 
  return m_game.getPlayerNames()[id];
}

void
BotClient::handleChatMessage(DOPE_SMARTPTR<ChatMessage> chatPtr)
{
  std::cout << chatPtr->sender << ">" << chatPtr->message << "\n";
}

void
BotClient::handleEndGame(DOPE_SMARTPTR<EndGame> egPtr)
{
  std::cout << "Game Over !\n";
  std::string msg;
  switch (egPtr->reason) {
  case 0:
    std::cout << "Server will quit.\n";
    m_quit=true;
    return;
    break;
  case 1:
    msg="*** Team ";
    if (egPtr->winner<m_game.getTeams().size())
      msg+=m_game.getTeams()[egPtr->winner].name;
    else
      msg+="?";
    msg+=" wins !! ***";
    break;
  case 2:
    msg="*** The game ends in a draw ***";
    break;
  }
  std::string stars(msg.size(),'*');
  std::cout << stars << "\n";
  std::cout << msg << "\n";
  std::cout << stars << "\n";
  // restart
  m_game.restart();
  m_playerIDs.clear();
  ClientGreeting g;
  g.m_userSetting=m_config.m_users;
  assert(m_streamPtr.get());
  m_streamPtr->so.emit(g);
}

bool
BotClient::connect()
{
  if (m_streamPtr.get()) {
    DOPE_WARN("already connected");
    return true;
  }
  try {
    m_streamPtr=DOPE_SMARTPTR<NetStream>(new NetStream(m_config.m_server,m_config.m_port));
  }catch(...){
    // todo: catch only socket errors and dns errors
    // dope should be fixed when errors occur in HostAddress
    if (m_streamPtr.get()) m_streamPtr=DOPE_SMARTPTR<NetStream>();
  }
  if (!m_streamPtr.get()) return false;

  m_streamPtr->si.connect(SigC::slot(*this,&BotClient::handleGreeting));
  m_streamPtr->si.connect(SigC::slot(*this,&BotClient::handleGame));
  m_streamPtr->si.connect(SigC::slot(*this,&BotClient::handlePlayerInput));
  m_streamPtr->si.connect(SigC::slot(*this,&BotClient::handleNewClient));
  m_streamPtr->si.connect(SigC::slot(*this,&BotClient::handleChatMessage));
  m_streamPtr->si.connect(SigC::slot(*this,&BotClient::handleEndGame));
  m_game.playerCollision.connect(SigC::slot(*this,&BotClient::handlePlayerCollision));
  m_game.wallCollision.connect(SigC::slot(*this,&BotClient::handleWallCollision));
  m_game.doorCollision.connect(SigC::slot(*this,&BotClient::handleDoorCollision));
  
  return true;
}

void
BotClient::sendGreeting()
{
  ClientGreeting g;
  g.m_userSetting=m_config.m_users;
  m_streamPtr->so.emit(g);
}

int
BotClient::main()
{
  DOPE_SMARTPTR<URICache<Mesh> > mc(new URICache<Mesh>());
  URILoader<URICache<Mesh> >::cache=mc.get();
  DOPE_SMARTPTR<URICache<PlayerData> > pdc(new URICache<PlayerData>());
  URILoader<URICache<PlayerData> >::cache=pdc.get();
  
#ifndef WINDOOF
  signal(SIGPIPE,sigPipeHandler);
#endif

  TimeStamp start;
  start.now();
  TimeStamp oldTime;
  TimeStamp newTime;
  TimeStamp stepSize(0,11111); // ~90Hz
  TimeStamp frameSize(stepSize);
  TimeStamp dt;
  TimeStamp null;
  TimeStamp timeOut;
  oldTime.now();
  unsigned frames=0;

  std::cout << "Welcome to ADIC !!!\n";
  if (!connect()) {
    std::cerr << "Could not connect to server\n";
    return 1;
  }
  sendGreeting();
  while (!m_quit) {
    newTime.now();
    dt=newTime-oldTime;
    while(dt<frameSize) {
      timeOut=(frameSize-dt);
      if (m_streamPtr.get()) {
	// we check for incoming messages here to reduce latency
	// for input messages
	if (m_streamPtr->select(&timeOut)) {
	  // read all messages
	  do {
	    m_streamPtr->read();
	  }while (m_streamPtr->select(&null));
	}
      }else{
	timeOut.sleep();
      }
      newTime.now();
      dt=newTime-oldTime;
    }
    frameSize=(dt-frameSize);
    int eframes=1;
    while (frameSize>stepSize) {
      ++eframes;
      frameSize-=stepSize;
    }
    frameSize=stepSize-frameSize;

    // start of one frame
    // do main work

    R rdt(R(stepSize.getSec())+R(stepSize.getUSec())/1000000);
    for (int f=0;f<eframes;++f) {
      m_game.step(rdt);
      // work on input queue
      InputQueue::iterator it(m_inputQueue.begin());
      while (it!=m_inputQueue.end()){
	if (m_game.getFrame()-(*it)->frame>=0) {
	  m_game.setInput(*(it->get()));
	  InputQueue::iterator del(it);
	  ++it;
	  m_inputQueue.erase(del);
	  continue;
	}
	++it;
      }
    }

    if (!step(rdt))
      m_quit=true;

    if (m_streamPtr.get()) m_streamPtr->readAll();

    // end of main work
    oldTime=newTime;
    ++frames;
    /*
    dt=newTime-start;
    R uptime=R(dt.getSec())+(R(dt.getUSec())/1000000);
    std::cout << "\rUptime: " << std::fixed << std::setprecision(2) << std::setw(10) << uptime 
	      << " FPS: " << std::setw(8) << R(frames)/uptime 
	      << " Frame: " << std::setw(10) << frames;*/
  }
  return 0;
}


bool
BotClient::step(R dt)
{
  for (unsigned i=0;i<m_bots.size();++i)
    m_bots[i]->step(dt);
  return true;
}

// dispatch events to correct PlayerBot

unsigned
BotClient::getPlayerBotID(PlayerID pid)
{
  const std::vector<PlayerID> &myids(getMyIDs());
  assert(myids.size()==m_bots.size());
  for (unsigned i=0;i<myids.size();++i) {
    if (myids[i]==pid) return i;
  }
  return ~0U;
}


void
BotClient::handlePlayerCollision(PlayerID p1, PlayerID p2, const V2D &cv)
{
  unsigned botid=getPlayerBotID(p1);
  if (botid!=~0U) {
    m_bots[botid]->playerCollision(p2,cv);
    return;
  }
  //  else
  botid=getPlayerBotID(p2);
  if (botid!=~0U) {
    m_bots[botid]->playerCollision(p1,cv);
    return;
  }
}

void
BotClient::handleWallCollision(PlayerID p, const std::vector<FWEdge::EID> &eids, const V2D &cv)
{
  unsigned botid=getPlayerBotID(p);
  if (botid==~0U) return;
  m_bots[botid]->wallCollision(eids,cv);
}

void
BotClient::handleDoorCollision(PlayerID p, unsigned did, const V2D &cv)
{
  unsigned botid=getPlayerBotID(p);
  if (botid==~0U) return;
  m_bots[botid]->doorCollision(did,cv);
}



int main(int argc,char *argv[])
{
  try{
    ArgvParser parser(argc,argv);
    BotClientConfig config;
    parser.simple(config,NULL);
    // exit if parser printed the help message
    if (parser.shouldExit()) return 1;
    config.setDefaults();
    if (!config.m_dataPath.empty()) dataPathPtr=&config.m_dataPath;
    BotClient client(config);
    return client.main();
    return 0;
  }
  catch (const std::exception &error){
    std::cerr << "\nUncaught std::exception: "<<error.what()<<std::endl;
  }
  catch (...){
    std::cerr << "\nUncaught unknown exception\n";
  }
  return 1;
}
