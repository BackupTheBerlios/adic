#include "gui.h"

#include "sdlglgui.h"

GUI*
GUIFactory::create(Client &client)
{
  const std::string &i(client.getConfig().m_gui.implementation);
  if (i=="SDLGLGUI")
    return new SDLGLGUI(client);
  DOPE_FATAL("Unkown implementation: \""<<i<<"\"");
}
