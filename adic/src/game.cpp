#include "game.h"

bool
Game::step(R dt)
{
  if (m_meshPtr.get()&&(!m_worldPtr.get()))
    m_worldPtr=WorldPtr(new World(*m_meshPtr.get()));
  return true;
}
