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

void sigPipeHandler(int x){
  std::cerr << "\nWARNING: Received sig pipe signal - I ignore it\n"<<std::endl;
}

int
Client::main()
{
  signal(SIGPIPE,sigPipeHandler);
  InternetAddress adr(HostAddress(m_config.m_server.c_str()),m_config.m_port);
  NetStreamBuf layer0(adr);
  layer0.setBlocking(false);
  OutProto l2out(layer0);
  InProto l2in(layer0,TimeStamp(0,300),5);
  SignalOutAdapter<OutProto> so(l2out);
  SignalInAdapter<InProto> si(l2in);
  si.connect(SigC::slot(*this,&Client::handleGreeting));
  si.connect(SigC::slot(*this,&Client::handleGame));
  si.connect(SigC::slot(*this,&Client::handlePlayerInput));

  TimeStamp start;
  start.now();
  TimeStamp oldTime;
  TimeStamp newTime;
  TimeStamp minStep(0,1000000/100); // max. 100 Hz will be less because of the min sleep problem
  TimeStamp dt;
  TimeStamp null;
  oldTime.now();
  unsigned frames=0;

  GUIFactory guif;
  GUI* guiPtr=guif.create(*this,m_config.m_gui);
  DOPE_CHECK(guiPtr->init());
  guiPtr->input.connect(SigC::slot(so,&SignalOutAdapter<OutProto>::emit<Input>));
  while (!m_quit) {
    // read all messages
    while (layer0.select(&null))
      si.read();
    newTime.now();
    dt=newTime-oldTime;
    while(dt<minStep) {
      (minStep-dt).sleep();
      newTime.now();
      dt=newTime-oldTime;
    }
    R rdt=R(dt.getSec())+R(dt.getUSec())/1000000;
    m_game.step(rdt);
    if (!guiPtr->step(rdt))
      m_quit=true;
    oldTime=newTime;
    ++frames;
    dt=newTime-start;
    R uptime=R(dt.getSec())+(R(dt.getUSec())/1000000);
    std::cout << "\rUptime: " << std::fixed << std::setprecision(2) << std::setw(15) << uptime 
	      << " FPS: " << std::setw(10) << R(frames)/uptime 
	      << " Frame: " << std::setw(20) << frames;
  }
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
