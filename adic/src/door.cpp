#include "door.h"
#include "wall.h"
//#include "world.h"

const R Door::damping=0.2;

bool 
Door::collide(const Circle &c, V2D &cv)
{
  // todo - can't collide without world !
  return false;
  /*
  Line l;
  Wall w(l);
  return w.collide(c,cv);
  */
}

bool
Door::isClosed() const
{
  assert(0);
  return true;
}
