#include "game.h"
#include "input.h"

bool
Game::step(R dt)
{
  for (unsigned p=0;p<m_players.size();++p)
    m_players[p].step(dt);
  return true;
}

void 
Game::setInput(const PlayerInput &i)
{
  if (i.id>=m_players.size())
    {
      DOPE_WARN("id out of range");
      return;
    }
  m_players[i.id].setControl(i.i.x,i.i.y);
}
