#include "door.h"
#include "wall.h"
//#include "world.h"

const R Door::damping=RAD(1);

bool
Door::isClosed() const
{
  return (angle<RAD(5))&&(angle>RAD(-5));
  return true;
}
