#include "bot.h"
#include "karmebot.h"

Bot *
BotFactory::create(BotClient &client)
{
  const std::string &i(client.getConfig().implementation);
  if (i=="karme")
    return new KarmeBot(client);
  DOPE_FATAL("Unkown implementation: \""<<i<<"\"");
}
