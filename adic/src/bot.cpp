#include "bot.h"
#include "karmebot.h"


void
Bot::sendInput()
{
  // emit input signal if we changed our controls
  if (cinput!=oinput) {
    input.emit(cinput);
    oinput=cinput;
    std::cerr << "I am " <<pid<<" and did send an input event\n";
  }
}


Bot *
BotFactory::create(BotClient &client,PlayerID _pid, unsigned _inputID)
{
  const std::string &i(client.getConfig().implementation);
  if (i=="KarmeBot")
    return new KarmeBot(client,_pid,_inputID);
  DOPE_FATAL("Unkown implementation: \""<<i<<"\"");
}
