#include "sdlmenu.h"

SDLMenu::SDLMenu(SDLGLGUI &_gui)
  : gui(_gui),
    gl(_gui.gl),
    config(gui.getConfig()),
    serverName(config.m_server), 
    serverPort(anyToString(config.m_port))
{
  // connect input fields to signal factory
  SDLSigFactory &sf(gui.getSigFactory());
  sf.keyEvent.connect(SigC::slot(serverName,&SDLInputField::handleKey));
  sf.keyEvent.connect(SigC::slot(serverPort,&SDLInputField::handleKey));
  // give focus to serverName input field
  serverName.setActive(true);
  // our method handleServerName should be called - if enter is pressed in the servername field
  serverName.input.connect(SigC::slot(*this,&SDLMenu::handleServerName));
  serverPort.input.connect(SigC::slot(*this,&SDLMenu::handleServerPort));
}

void
SDLMenu::handleInput(Input &i)
{
}

void
SDLMenu::handleServerName(const std::string &name)
{
  serverPort.setActive(true);
}

void
SDLMenu::handleServerPort(const std::string &port)
{
  unsigned short int p;
  try {
    stringToAny(port,p);
  }catch(StringConversion){
    // could not convert string to number
    serverPort.setContent(anyToString(config.m_port));
    serverPort.setActive(true);
    return;
  }
  config.m_server=serverName.getContent();
  config.m_port=p;
  if (!serverSelected.emit()) {
    std::cout << "Could not connect to server\n";
    // let the user enter the hostname again
    serverName.setActive(true);
  }
}


bool
SDLMenu::step(R dt)
{
  if ((!serverPort.isActive())&&(!serverName.isActive()))
    return false;
  
  // get width and height (Attention: this may change from step to step since
  // the window may be resized
  int w=gui.getWidth();
  int h=gui.getHeight();

  // draw server name and port input field
  gl.LoadIdentity();
  gl.Translatef(float(w/2),float(h/2),0);
  if (serverName.isActive())
    gl.Color3f(0.0,1.0,0.0);
  else
    gl.Color3f(1.0,1.0,1.0);
  gui.m_fontPtr->drawText(serverName.getContent(),true);
  gl.Translatef(0,-gui.m_fontPtr->getHeight(),0);
  if (serverPort.isActive())
    gl.Color3f(0.0,1.0,0.0);
  else
    gl.Color3f(1.0,1.0,1.0);
  gui.m_fontPtr->drawText(serverPort.getContent(),true);
  return true;
}

