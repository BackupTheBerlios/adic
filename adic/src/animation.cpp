#include "animation.h"
#include "sdlglgui.h"

Animation::Animation(SDLGLGUI &gui, const std::vector<std::string> &uris)
  : time(0)
{
  for (unsigned i=0;i<uris.size();++i)
    textures.push_back(gui.getTexture(uris[i]));
}

Texture &
Animation::getTexture() const
{
  unsigned id=unsigned(fabs(time));
  assert(id<textures.size());
  assert(textures[id].get());
  return *textures[id].get();
}

void 
Animation::step(R dt)
{
  if (textures.size()<2) return;
  time+=dt;
  while (time>=textures.size())
	time-=textures.size();
  while (time<0)
    time+=textures.size();
  assert(time>=0);
  assert(time<textures.size());
}

