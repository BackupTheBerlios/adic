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

#include "client.h"
#include "gui.h"
#include "sound.h"

void sigPipeHandler(int x){
  std::cerr << "\nWARNING: Received sig pipe signal - I ignore it\n"<<std::endl;
}


void
Client::handleGreeting(DOPE_SMARTPTR<ServerGreeting> gPtr)
{
  assert(gPtr.get());
  std::cerr << "\nGot Greeting from server "<< gPtr->m_adicVersion.asString() << " DOPE++ "<<gPtr->m_dopeVersion.asString()<<"\n";
  m_playerIDs=gPtr->m_players;
  std::cerr << "Got "<<m_playerIDs.size()<<" player IDs:";
  for (unsigned i=0;i<m_playerIDs.size();++i)
    std::cerr << "\n"<<m_playerIDs[i];
  std::cerr << "\n";
}

void 
Client::handleGame(DOPE_SMARTPTR<Game> gPtr)
{
  assert(gPtr.get());
  //  std::cerr << "\nGot game data\n";
  TimeStamp myTime(m_game.getTimeStamp());
  TimeStamp serverTime(gPtr->getTimeStamp());
  /* lag compensation
  if (serverTime<myTime) {
    // packet lag
    TimeStamp lag(myTime-serverTime);
    R dt=lag.getSec()+R(lag.getUSec())/1000000;
    std::cerr << "\nLag: "<<dt<<" sec.\n";
    gPtr->step(dt);
    }*/
  // todo later we will perhaps need a replace method
  // backup worldPtr - otherwise it would be verry time-consuming (build world from mesh ...)
  Game::WorldPtr w=m_game.getWorldPtr();
  //  std::cerr << "\nGot Game with "<<gPtr->getPlayers().size() << " players (was: "<<m_game.getPlayers().size()<<")\n";
  m_game=*gPtr.get();
  //  std::cerr << "after copy have "<<m_game.getPlayers().size()<<" players\n";
  m_game.setWorldPtr(w);
  m_game.collision.connect(SigC::slot(*this,&Client::handleCollision));
}

void 
Client::handleCollision(V2D pos, R strength)
{
  if (!m_soundPtr)
    return;
  int c=m_soundPtr->playSample("data/collision.wav");
  R volume=strength/40;
  if (volume>1) volume=1;
  m_soundPtr->modifyChannel(c,volume);
  //  std::cerr << "\nPlay sample\n";
}

void
Client::handlePlayerInput(DOPE_SMARTPTR<PlayerInput> iPtr)
{
  DOPE_CHECK(iPtr.get());
  m_game.setInput(*iPtr.get());
    //    std::cerr << "\nGot Input\n";
}

int
Client::main()
{
  signal(SIGPIPE,sigPipeHandler);
  InternetAddress adr(HostAddress(m_config.m_server.c_str()),m_config.m_port);
  NetStreamBuf layer0(adr);
  layer0.setBlocking(false);
  OutProto l2out(layer0);
#if USE_RAW_PROTOCOL == 1
  InProto l2in(layer0);
#elif USE_XML_PROTOCOL == 1
  InProto l2in(layer0,TimeStamp(0,300),2);
#endif
  SignalOutAdapter<OutProto> so(l2out);
  SignalInAdapter<InProto> si(l2in);
  si.connect(SigC::slot(*this,&Client::handleGreeting));
  si.connect(SigC::slot(*this,&Client::handleGame));
  si.connect(SigC::slot(*this,&Client::handlePlayerInput));

  ClientGreeting g;
  g.m_userSetting=m_config.m_users;
  so.emit(g);

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

  GUIFactory guif;
  GUI* guiPtr=guif.create(*this,m_config.m_gui);
  m_soundPtr=Sound::create(m_config.m_sc);
  m_soundPtr->playMusic("data/music.mod");

  DOPE_CHECK(guiPtr->init());
  guiPtr->input.connect(SigC::slot(so,&SignalOutAdapter<OutProto>::emit<Input>));
  while (!m_quit) {
    newTime.now();
    dt=newTime-oldTime;
    while(dt<minStep) {
      timeOut=(minStep-dt);
      if (layer0.select(&timeOut)) {
	// read all messages
	do {
	  si.read();
	}while (layer0.select(&null));
      }
      newTime.now();
      dt=newTime-oldTime;
    }
    // do main work
    R rdt=R(dt.getSec())+R(dt.getUSec())/1000000;
    m_game.step(rdt);
    if (!guiPtr->step(rdt))
      m_quit=true;
    m_soundPtr->step(rdt);
    while (layer0.select(&null))
      si.read();
    // end of main work
    oldTime=newTime;
    ++frames;
    dt=newTime-start;
    R uptime=R(dt.getSec())+(R(dt.getUSec())/1000000);
    std::cout << "\rUptime: " << std::fixed << std::setprecision(2) << std::setw(10) << uptime 
	      << " FPS: " << std::setw(8) << R(frames)/uptime 
	      << " Frame: " << std::setw(10) << frames;
  }
  delete m_soundPtr;
  m_soundPtr=NULL;
  delete guiPtr;
  guiPtr=NULL;
  std::cout << std::endl;
  return 0;
}

int main(int argc,char *argv[])
{
  try{
    ArgvParser parser(argc,argv);
    ClientConfig config;
    parser.simple(config,NULL);
    // exit if parser printed the help message
    if (parser.shouldExit()) return 1;
    Client client(config);
    return client.main();

    return 0;
  }
  catch (const std::exception &error){
    std::cerr << "\nUncaught std::exception: "<<error.what()<<std::endl;
  }
  return 1;
}
