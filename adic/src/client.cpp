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

#include "client.h"
#include "gui.h"
#include "sound.h"

#include <dope/argvparser.h> // layer 2 argument vector parser
#include <dope/dopestl.h>

void sigPipeHandler(int x){
  std::cerr << "\nWARNING: Received sig pipe signal - I ignore it\n"<<std::endl;
}


Client::Client(GUIClientConfig &config) 
  : m_config(config), m_quit(false), m_csong(0),
    m_cerrbuf(NULL), m_coutbuf(NULL)
{
  m_songs.push_back("music-eric2.s3m");
  m_songs.push_back("music-eric.s3m");
  m_songs.push_back("music1.xm");
  // todo add some songs here - or better put the list into clientconfig
}
Client::~Client() 
{
  if (m_cerrbuf) {
    std::cerr.rdbuf(m_cerrbuf);
    m_cerrbuf=NULL;
  }
  if (m_coutbuf) {
    std::cout.rdbuf(m_coutbuf);
    m_coutbuf=NULL;
  }
}

void
Client::handleGreeting(DOPE_SMARTPTR<ServerGreeting> gPtr)
{
  DOPE_ASSERT(gPtr.get());
  std::cout << "\nGot Greeting from server.\nServer is running ver. "<< gPtr->m_adicVersion.asString() << "\n";
  std::cout << "(DOPE++ ver. "<<gPtr->m_dopeVersion.asString()<<")\n";
  m_playerIDs=gPtr->m_players;

  unsigned got=m_playerIDs.size();
  unsigned req=m_config.m_users.users.size();
  DOPE_ASSERT(m_guiPtr.get());
  unsigned devs=m_guiPtr->numInputDevices();
  std::cout << "I Got "<< got <<" player IDs (requested "<< req <<" )\n";
  std::cout << "I found "<<devs<<" input devices\n";
  if ((req>0)&&(!got))
    std::cout << "The server is full.\n";
  playNextSong();
  m_guiPtr->handleGreeting(gPtr);
}

void 
Client::handleGame(DOPE_SMARTPTR<Game> gPtr)
{
  DOPE_ASSERT(gPtr.get());
  m_game.replace(*gPtr,m_config.m_lagCompensation);
  //  reconnect signals
  m_game.collision.connect(SigC::slot(*this,&Client::handleCollision));
}

void 
Client::handleCollision(V2D pos, R strength)
{
  if (m_soundPtr.get()) {
    // collision sound 
    R volume=strength/300;
    // zoom should be used too
    if (m_guiPtr.get()) volume-=(m_guiPtr->getPos()-pos).length()/R(10000);
    if (volume>0.2) {
      if (volume>0.75) volume=0.75;
      int c=m_soundPtr->playSample("collision.wav");
      m_soundPtr->modifyChannel(c,volume);
    }
  }
}

void
Client::handlePlayerInput(DOPE_SMARTPTR<PlayerInput> iPtr)
{
  DOPE_ASSERT(iPtr.get());
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
Client::handleNewClient(DOPE_SMARTPTR<NewClient> mPtr)
{
  m_game.setPlayerNames(mPtr->playerNames);
  m_game.setTeams(mPtr->teams);
  DOPE_ASSERT(m_guiPtr.get());
  m_guiPtr->handleNewClient(mPtr);
}

std::string
Client::getPlayerName(PlayerID id) const
{
  if (id>=m_game.getPlayerNames().size()) return anyToString(id);
  //else 
  return m_game.getPlayerNames()[id];
}

void 
Client::playNextSong()
{
  if (!m_soundPtr.get())
    return;
  if (m_csong<m_songs.size()) {
    m_soundPtr->playMusic(m_songs[m_csong].c_str(),0.8,-1);
    ++m_csong;
    if (m_csong>=m_songs.size())
      m_csong=0;
  }
}

void
Client::printed(char c)
{
  if (!m_soundPtr.get())
    return;
  switch (c) {
  case '\n':
    m_soundPtr->modifyChannel(m_soundPtr->playSample("newline.wav"),0.5);
    break;
  default:
    int ch=m_soundPtr->playSample("printer.wav");
    R volume=0.8-R(c%6)/(5*2);
    m_soundPtr->modifyChannel(ch,volume);
    break;
  }
}

void
Client::handleChatMessage(DOPE_SMARTPTR<ChatMessage> chatPtr)
{
  std::cout << chatPtr->sender << ">" << chatPtr->message << "\n";
}

void
Client::handleEndGame(DOPE_SMARTPTR<EndGame> egPtr)
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

  DOPE_ASSERT(m_guiPtr.get());
  m_guiPtr->handleEndGame(egPtr);

  // send greeting again
  ClientGreeting g;
  g.m_userSetting=m_config.m_users;
  DOPE_ASSERT(m_streamPtr.get());
  m_streamPtr->so.emit(g);
}

void
Client::handlePing(DOPE_SMARTPTR<Ping> pingPtr)
{
  TimeStamp pt;
  pt.now();
  pt-=pingPtr->m_ctime;
  std::cout << "Ping time: "<<pt.getSec()<<"sec and "<<pt.getUSec()<<"usec\n";
}

bool
Client::connect()
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
  m_streamPtr->layer0.setTcpNoDelay(m_config.m_tcpNoDelay);
  m_streamPtr->si.connect(SigC::slot(*this,&Client::handleGreeting));
  m_streamPtr->si.connect(SigC::slot(*this,&Client::handleGame));
  m_streamPtr->si.connect(SigC::slot(*this,&Client::handlePlayerInput));
  m_streamPtr->si.connect(SigC::slot(*this,&Client::handleNewClient));
  m_streamPtr->si.connect(SigC::slot(*this,&Client::handleChatMessage));
  m_streamPtr->si.connect(SigC::slot(*this,&Client::handleEndGame));
  m_streamPtr->si.connect(SigC::slot(*this,&Client::handlePing));

  DOPE_ASSERT(m_guiPtr.get());
  m_guiPtr->input.connect(SigC::slot(m_streamPtr->so,&SignalOutAdapter<OutProto>::emit<Input>));
  m_guiPtr->chatMessage.connect(SigC::slot(m_streamPtr->so,&SignalOutAdapter<OutProto>::emit<ChatMessage>));

  // todo: in the moment dope - does not allow to connect twice
  //  m_streamPtr->si.connect(SigC::slot(*m_guiPtr,&GUI::handleEndGame));

  return true;
}

void
Client::sendGreeting()
{
  ClientGreeting g;
  g.m_userSetting=m_config.m_users;
  m_streamPtr->so.emit(g);
}

void
Client::ping()
{
  Ping p;
  p.m_ctime.now();
  m_streamPtr->so.emit(p);
}

int
Client::main()
{
  DOPE_SMARTPTR<URICache<Mesh> > mc(new URICache<Mesh>());
  URILoader<URICache<Mesh> >::cache=mc.get();
  DOPE_SMARTPTR<URICache<PlayerData> > pdc(new URICache<PlayerData>());
  URILoader<URICache<PlayerData> >::cache=pdc.get();

  Player::m_defaultDataPtr.setAddress("data:default.xml");
  
#ifndef WINDOOF
  signal(SIGPIPE,sigPipeHandler);
#endif

  /*
    InternetAddress adr(HostAddress(m_config.m_server.c_str()),m_config.m_port);
    NetStreamBuf layer0(adr);
    //  layer0.setBlocking(false);
    OutProto l2out(layer0);
    #if USE_RAW_PROTOCOL == 1
    InProto l2in(layer0);
    #elif USE_XML_PROTOCOL == 1
    InProto l2in(layer0,TimeStamp(0,300),2);
    #endif
    SignalOutAdapter<OutProto> so(l2out);
    soPtr=&so;

    ClientGreeting g;
    g.m_userSetting=m_config.m_users;
    so.emit(g);
  */

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

  GUIFactory guif;
  m_guiPtr=DOPE_SMARTPTR<GUI>(guif.create(*this));
  /*
    m_cerrbuf=std::cerr.rdbuf();
    std::cerr.rdbuf(m_guiPtr->getOstream().rdbuf());
  */
  m_coutbuf=std::cout.rdbuf();
  std::cout.rdbuf(m_guiPtr->rdbuf());
  m_soundPtr=DOPE_SMARTPTR<Sound>(Sound::create(m_config.m_sc));
  if (m_soundPtr.get())
    m_soundPtr->musicFinished.connect(SigC::slot(*this,&Client::playNextSong));
  playNextSong();
  m_guiPtr->printed.connect(SigC::slot(*this,&Client::printed));
  
  std::vector<int> soundChannel;
  
  DOPE_CHECK(m_guiPtr->init());

  std::cout << "Welcome to ADIC !!!\n";
  std::cout << "Please report bugs and give us some feedback !\nadic-general@lists.berlios.de (http://adic.berlios.de)\n";
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

#ifdef ADIC_DEBUG_TIMING
    // todo remove again
    // last frame size was dt
    // and it should have been frameSize
    std::cerr << "\nLast frame took: "
	      << (R(dt.getSec())+(R(dt.getUSec())/1000000))
	      << " and should have taken: "
	      << (R(frameSize.getSec())+(R(frameSize.getUSec())/1000000));
#endif

    frameSize=(dt-frameSize);
    // the new framesize should be stepSize-frameSize
    int eframes=1;
    // if this would be negative we skip some frames
    while (frameSize>stepSize) {
      ++eframes;
      frameSize-=stepSize;
    }
    frameSize=stepSize-frameSize;

#ifdef ADIC_DEBUG_TIMING
    // todo remove again
    std::cerr << "\nNext frame size: "<< (R(frameSize.getSec())+(R(frameSize.getUSec())/1000000));
    if (eframes>1) {
      DOPE_WARN(" (machine too slow: calculate "<<eframes<<" frames at once)");
    }
#endif

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
    

    // gui
    if (!m_guiPtr->step(rdt*eframes))
      m_quit=true;
    // sound
    if (m_soundPtr.get()) {
      /*
      // walking sound
      for (unsigned i=0;i<m_playerIDs.size();++i) {
	const Player &p(m_game.getPlayers()[i]);
	while (soundChannel.size()<i)
	  soundChannel.push_back(-1);
	int &c=soundChannel[i];
	if (!p.getY()) {
	  if (c!=-1)
	    m_soundPtr->stopChannel(c);
	  continue;
	}
	V2D pos(p.m_pos);
	R volume=R(1)-(m_guiPtr->getPos()-pos).length()/R(200);
	if (volume>0) {
	  if (volume>1) volume=1;
	  //  std::cerr << "\nPlay sample\n";
	  if (c==-1)
	    c=m_soundPtr->playSample("Footsteps.wav",-1);
	  m_soundPtr->modifyChannel(c,volume);
	}else if (c!=-1)
	  m_soundPtr->stopChannel(c);
      }
      */
      m_soundPtr->step(rdt*eframes);
    }
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
  m_soundPtr=DOPE_SMARTPTR<Sound>();
  m_guiPtr=DOPE_SMARTPTR<GUI>();
  return 0;
}

#ifndef ADIC_NEED_SDLMAIN
int main(int argc,char *argv[])
#else

int cppmain(int argc,char *argv[]);

extern "C" {
  int SDL_main(int argc,char *argv[])
  {
    return cppmain(argc,argv);
  }
}

int cppmain(int argc,char *argv[])
#endif
{
  try{
    ArgvParser parser(argc,argv);
    GUIClientConfig config;
    parser.simple(config,NULL);
    // exit if parser printed the help message
    if (parser.shouldExit()) return 1;
    config.setDefaults();
    if (!config.m_dataPath.empty()) dataPathPtr=&config.m_dataPath;
    Client client(config);
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
