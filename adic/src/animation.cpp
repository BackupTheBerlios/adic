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
  DOPE_ASSERT(id<textures.size());
  DOPE_ASSERT(textures[id].get());
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
  DOPE_ASSERT(time>=0);
  DOPE_ASSERT(time<textures.size());
}

