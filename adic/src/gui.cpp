#include "gui.h"

#include "sdlglgui.h"

GUI*
GUIFactory::create(Client &client, const GUIConfig &config)
{
  if (config.implementation=="SDLGLGUI")
    return new SDLGLGUI(client,config);
  DOPE_FATAL("Unkown implementation: \""<<config.implementation<<"\"");
}
