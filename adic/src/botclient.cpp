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

#include "botclient.h"
#include "bot.h"

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
  /*
    for (unsigned i=0;i<m_playerIDs.size();++i)
    std::cerr << "\n"<<m_playerIDs[i];
    std::cerr << "\n";
  */
  assert(m_botPtr.get());
  m_botPtr->startGame();
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
  DOPE_CHECK(iPtr.get());
  m_game.setInput(*iPtr.get());
    //    std::cerr << "\nGot Input\n";
}

void
BotClient::handleNewClient(DOPE_SMARTPTR<NewClient> mPtr)
{
  /*
  std::cerr << "\ngot player names:\n";
  for (unsigned i=0;i<mPtr->playerNames.size();++i)
  std::cerr << mPtr->playerNames[i] << std::endl;*/
  m_game.setPlayerNames(mPtr->playerNames);
  m_game.setTeams(mPtr->teams);
  DOPE_CHECK(m_botPtr.get());
  m_botPtr->handleNewClient(mPtr);
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
  DOPE_CHECK(m_streamPtr.get());
  m_streamPtr->so.emit(g);
  assert(m_botPtr.get());
  m_botPtr->endGame();
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
  DOPE_CHECK(m_botPtr.get());
  m_botPtr->input.connect(SigC::slot(m_streamPtr->so,&SignalOutAdapter<OutProto>::emit<Input>));
  m_botPtr->chatMessage.connect(SigC::slot(m_streamPtr->so,&SignalOutAdapter<OutProto>::emit<ChatMessage>));
  m_game.playerCollision.connect(SigC::slot(*m_botPtr,&Bot::handlePlayerCollision));
  m_game.wallCollision.connect(SigC::slot(*m_botPtr,&Bot::handleWallCollision));
  m_game.doorCollision.connect(SigC::slot(*m_botPtr,&Bot::handleDoorCollision));
  
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
  
#ifndef WIN32
  signal(SIGPIPE,sigPipeHandler);
#endif

  TimeStamp start;
  start.now();
  TimeStamp oldTime;
  TimeStamp newTime;
  TimeStamp minStep(0,1000000/100); // max. 100 Hz will be less (ca. 50) because of the min sleep problem
  TimeStamp dt;
  TimeStamp null;
  TimeStamp timeOut;
  oldTime.now();
  unsigned frames=0;

  BotFactory factory;
  m_botPtr=DOPE_SMARTPTR<Bot>(factory.create(*this));

  std::cout << "Welcome to ADIC !!!\n";
  if (!connect()) {
    std::cerr << "Could not connect to server\n";
    return 1;
  }
  sendGreeting();
  while (!m_quit) {
    newTime.now();
    dt=newTime-oldTime;
    while(dt<minStep) {
      timeOut=(minStep-dt);
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
    // do main work
    R rdt=R(dt.getSec())+R(dt.getUSec())/1000000;
    // game
    m_game.step(rdt);
    if (!m_botPtr->step(rdt))
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
  m_botPtr=DOPE_SMARTPTR<Bot>();
  return 0;
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
