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


Client::Client(ClientConfig &config) 
  : m_config(config), m_quit(false), m_soundPtr(NULL), m_csong(0), m_guiPtr(NULL),
    m_cerrbuf(NULL), m_coutbuf(NULL)
{
  m_songs.push_back("data/music.mod");
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
  assert(gPtr.get());
  std::cout << "\nGot Greeting from server.\nServer is running ver. "<< gPtr->m_adicVersion.asString() << "\n";
  std::cout << "(DOPE++ ver. "<<gPtr->m_dopeVersion.asString()<<")\n";
  m_playerIDs=gPtr->m_players;

  unsigned got=m_playerIDs.size();
  unsigned req=m_config.m_users.users.size();
  unsigned devs=m_guiPtr->numInputDevices();
  std::cout << "I Got "<< got <<" player IDs (requested "<< req <<" )\n";
  std::cout << "I found "<<devs<<" input devices\n";
  if ((req>0)&&(!got))
    std::cout << "The server is full.\n";
  /*
    for (unsigned i=0;i<m_playerIDs.size();++i)
    std::cerr << "\n"<<m_playerIDs[i];
    std::cerr << "\n";
  */
}

void 
Client::handleGame(DOPE_SMARTPTR<Game> gPtr)
{
  assert(gPtr.get());
  m_game.replace(*gPtr);
  //  reconnect signals
  m_game.collision.connect(SigC::slot(*this,&Client::handleCollision));
}

void 
Client::handleCollision(V2D pos, R strength)
{
  if (m_soundPtr.get()) {
    // collision sound 
    R volume=strength/150;
    if (m_guiPtr.get()) volume-=(m_guiPtr->getPos()-pos).length()/R(1000);
    if (volume>0) {
      if (volume>1) volume=1;
      //  std::cerr << "\nPlay sample\n";
      int c=m_soundPtr->playSample("data/collision.wav");
      m_soundPtr->modifyChannel(c,volume);
    }
  }
}

void
Client::handlePlayerInput(DOPE_SMARTPTR<PlayerInput> iPtr)
{
  DOPE_CHECK(iPtr.get());
  m_game.setInput(*iPtr.get());
    //    std::cerr << "\nGot Input\n";
}

void
Client::handleNewClient(DOPE_SMARTPTR<NewClient> mPtr)
{
  std::cerr << "\ngot player names:\n";
  for (unsigned i=0;i<mPtr->playerNames.size();++i)
    std::cerr << mPtr->playerNames[i] << std::endl;
  m_game.setPlayerNames(mPtr->playerNames);
  m_game.setTeams(mPtr->teams);
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
    m_soundPtr->playMusic(m_songs[m_csong].c_str());
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
  case ' ':
    m_soundPtr->playSample("data/newline.wav");
    break;
  default:
    int ch=m_soundPtr->playSample("data/printer.wav");
    R volume=1.0-R(c%6)/7;
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
  ClientGreeting g;
  g.m_userSetting=m_config.m_users;
  assert(soPtr);
  soPtr->emit(g);
}


int
Client::main()
{
  DOPE_SMARTPTR<URICache<Mesh> > mc(new URICache<Mesh>());
  URILoader<URICache<Mesh> >::cache=mc.get();
  DOPE_SMARTPTR<URICache<PlayerData> > pdc(new URICache<PlayerData>());
  URILoader<URICache<PlayerData> >::cache=pdc.get();
  
  signal(SIGPIPE,sigPipeHandler);
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
  SignalInAdapter<InProto> si(l2in);
  si.connect(SigC::slot(*this,&Client::handleGreeting));
  si.connect(SigC::slot(*this,&Client::handleGame));
  si.connect(SigC::slot(*this,&Client::handlePlayerInput));
  si.connect(SigC::slot(*this,&Client::handleNewClient));
  si.connect(SigC::slot(*this,&Client::handleChatMessage));
  si.connect(SigC::slot(*this,&Client::handleEndGame));

  ClientGreeting g;
  g.m_userSetting=m_config.m_users;
  so.emit(g);

  TimeStamp start;
  start.now();
  TimeStamp oldTime;
  TimeStamp newTime;
  TimeStamp minStep(0,1000000/30); // max. 30 Hz will be less because of the min sleep problem
  TimeStamp dt;
  TimeStamp null;
  TimeStamp timeOut;
  oldTime.now();
  unsigned frames=0;

  GUIFactory guif;
  m_guiPtr=DOPE_SMARTPTR<GUI>(guif.create(*this,m_config.m_gui));
  /*
    m_cerrbuf=std::cerr.rdbuf();
    std::cerr.rdbuf(m_guiPtr->getOstream().rdbuf());
  */
  m_coutbuf=std::cout.rdbuf();
  std::cout.rdbuf(m_guiPtr->getOstream().rdbuf());
  m_soundPtr=DOPE_SMARTPTR<Sound>(Sound::create(m_config.m_sc));
  if (m_soundPtr.get())
    m_soundPtr->musicFinished.connect(SigC::slot(*this,&Client::playNextSong));
  playNextSong();
  m_guiPtr->printed.connect(SigC::slot(*this,&Client::printed));
  
  std::vector<int> soundChannel;
  
  DOPE_CHECK(m_guiPtr->init());
  m_guiPtr->input.connect(SigC::slot(so,&SignalOutAdapter<OutProto>::emit<Input>));
  m_guiPtr->chatMessage.connect(SigC::slot(so,&SignalOutAdapter<OutProto>::emit<ChatMessage>));
  std::cout << "Welcome to ADIC !!!\n";
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
    // game
    m_game.step(rdt);
    // gui
    if (!m_guiPtr->step(rdt))
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
	    c=m_soundPtr->playSample("data/Footsteps.wav",-1);
	  m_soundPtr->modifyChannel(c,volume);
	}else if (c!=-1)
	  m_soundPtr->stopChannel(c);
      }
      */
      m_soundPtr->step(rdt);
    }
    while (layer0.select(&null))
      si.read();
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
  soPtr=NULL;
  m_soundPtr=DOPE_SMARTPTR<Sound>(NULL);
  m_guiPtr=DOPE_SMARTPTR<GUI>(NULL);
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
    config.setDefaults();
    Client client(config);
    return client.main();

    return 0;
  }
  catch (const std::exception &error){
    std::cerr << "\nUncaught std::exception: "<<error.what()<<std::endl;
  }
  return 1;
}
