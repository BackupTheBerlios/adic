#include "sdlmenu.h"

static int player_i;
static bool step_return = true;

const std::string file_names[] = {"data/gui_0001.png", "data/gui_0002.png", "data/gui_0003.png", "data/gui_0004.png", "data/gui_0005.png", "data/gui_0006.png", "data/gui_0007.png" };

SDLMenu::SDLMenu(SDLGLGUI &_gui)
  : gui(_gui),
    gl(_gui.gl),
    config(gui.getConfig()),
    serverName(config.m_server), 
    serverPort(anyToString(config.m_port)),
    playerName(""),
    teamName(""),
    m_screen(0),
    m_playerNum(1)
{
  // connect input fields to signal factory
  SDLSigFactory &sf(gui.getSigFactory());
  sf.keyEvent.connect(SigC::slot(serverName,&SDLInputField::handleKey));
  sf.keyEvent.connect(SigC::slot(serverPort,&SDLInputField::handleKey));
  sf.keyEvent.connect(SigC::slot(playerName,&SDLInputField::handleKey));
  sf.keyEvent.connect(SigC::slot(teamName,&SDLInputField::handleKey));
  // our method handleServerName should be called - if enter is pressed in the servername field
  serverName.input.connect(SigC::slot(*this,&SDLMenu::handleServerName));
  serverPort.input.connect(SigC::slot(*this,&SDLMenu::handleServerPort));
  playerName.input.connect(SigC::slot(*this,&SDLMenu::handlePlayerName));
  teamName.input.connect(SigC::slot(*this,&SDLMenu::handleTeamName));

  m_hiPtr = DOPE_SMARTPTR<Texture>(gui.getTexture("data/hi.png"));
  for (int i = 0; i<7; i++)
    {
      m_screens.push_back( DOPE_SMARTPTR<Texture>(gui.getTexture(file_names[i])) );
    }
}

void
SDLMenu::handleInput(Input &i)
{
  switch (m_screen)
    {
    case 0:
      // screen: "welcome", next: "choose server"
      serverName.setActive(true);
      m_screen = 1;
      i.x = 0;
      i.y = 0;
      break;
    case 2:
      // screen: "number of players", next: "player data"
      // std::cerr << "users: " << config.m_users.users.size() << std::endl;
      if (i.y < 0)
	{
	  if (m_playerNum<4) m_playerNum++;
	}
      else if (i.y > 0)
	{
	  if (m_playerNum>1) m_playerNum--;
	}
      if (i.x > 0)
	{
	  playerName.setContent(config.m_users.users[0].m_uname);
	  teamName.setContent(config.m_users.users[0].m_tname);
	  playerName.setActive(true);
	  m_screen = 3;
	  player_i = 0;
	  i.x = 0;
	  i.y = 0;
	}
      break;
    case 4:
      // screen: "adic info", next: start game
      save.emit();
      step_return = false;
      finish();
      break;
    case 5:
      // screen: "server error - conn", next: "choose server"
      serverName.setActive(true);
      m_screen = 1;
      i.x = 0;
      i.y = 0;
      break;
    case 6:
      // screen: "server error - full", not implemented yet
      break;
    }
}

void
SDLMenu::handleNewClient(DOPE_SMARTPTR<NewClient> mPtr)
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
    // let the user enter the hostname again
    // show error screen
    m_screen = 5;
  }
  else
    // goto screen "number of players"
    m_screen = 2;
}

void
SDLMenu::handlePlayerName(const std::string &name)
{
  teamName.setActive(true);
}

void
SDLMenu::handleTeamName(const std::string &name)
{
  //serverPort.setActive(true);
  if (player_i < config.m_users.users.size())
    {
      config.m_users.users[player_i].m_uname = playerName.getContent();
      config.m_users.users[player_i].m_tname = teamName.getContent();
    }
  else
    {
      config.m_users.users.push_back(User());
      config.m_users.users[config.m_users.users.size()-1].m_uname = playerName.getContent();
      config.m_users.users[config.m_users.users.size()-1].m_tname = teamName.getContent();
    }
  if (player_i+1 < m_playerNum)
    {
      playerName.setContent("");
      playerName.setActive(true);
      player_i++;
    }
  else
    {
      if (!serverSelected.emit())
	m_screen = 5;
      else
	m_screen = 4;
    }
  std::cerr << "users: " << config.m_users.users.size() << std::endl;
}

bool
SDLMenu::step(R dt)
{
  //if ((!serverPort.isActive())&&(!serverName.isActive()))
  //  return false;
  
  // get width and height (Attention: this may change from step to step since
  // the window may be resized
  int w=gui.getWidth();
  int h=gui.getHeight();

  // draw GUI background
  gui.drawTexture(*m_screens[m_screen].get(),V2D(float(w/2),float(h/2)), float(M_PI/2));

  // draw server name and port input field
  switch (m_screen) {

  case 0:
    // Screen 0: "Welcome", nothing to do
    break;

  case 1:
    // Screen 1: "Choose Server", hilite Inputfield
    gl.LoadIdentity();
    if (serverName.isActive())
      gui.drawTexture(*m_hiPtr.get(),V2D(float(w/2),float(h/2)+26), float(M_PI/2));
    else
      gui.drawTexture(*m_hiPtr.get(),V2D(float(w/2),float(h/2)-25), float(M_PI/2));
    gl.Translatef(float(w/2),float(h/2)+18,0);
    if (serverName.isActive())
      gl.Color3f(0.0,1.0,0.0);
    else
      gl.Color3f(1.0,1.0,1.0);
    gui.m_fontPtr->drawText(serverName.getContent(),true);
    gl.Translatef(0,-gui.m_fontPtr->getHeight()-34,0);
    if (serverPort.isActive())
      gl.Color3f(0.0,1.0,0.0);
    else
      gl.Color3f(1.0,1.0,1.0);
    gui.m_fontPtr->drawText(serverPort.getContent(),true);
    break;
  case 2:
    // Screen 2: "Number of Players", hilite Selection
    gl.LoadIdentity();
    gui.drawTexture(*m_hiPtr.get(),V2D(float(w/2),float(h/2)+90-m_playerNum*40), float(M_PI/2));
    break;
  case 3:
    // Screen 3: Player Data
    gl.LoadIdentity();
    if (playerName.isActive())
      gui.drawTexture(*m_hiPtr.get(),V2D(float(w/2),float(h/2)+26), float(M_PI/2));
    else
      gui.drawTexture(*m_hiPtr.get(),V2D(float(w/2),float(h/2)-25), float(M_PI/2));
    gl.Translatef(float(w/2),float(h/2)+58,0);
    gl.Color3f(1.0,1.0,1.0);
    gui.m_fontPtr->drawText("Player " + anyToString(player_i+1),true);

    gl.Translatef(0, -40,0);
    if (playerName.isActive())
      gl.Color3f(0.0,1.0,0.0);
    else
      gl.Color3f(1.0,1.0,1.0);
    gui.m_fontPtr->drawText(playerName.getContent(),true);
    gl.Translatef(0,-gui.m_fontPtr->getHeight()-34,0);
    if (teamName.isActive())
      gl.Color3f(0.0,1.0,0.0);
    else
      gl.Color3f(1.0,1.0,1.0);
    gui.m_fontPtr->drawText(teamName.getContent(),true);
    gl.Translatef(0,-gui.m_fontPtr->getHeight()-34,0);
    break;

  }
  return step_return;
}

