#include "door.h"
#include "wall.h"
//#include "world.h"

const R Door::damping=RAD(3);

Door::Door(FWEdge::EID _eid)
  : angle(0), angleSpeed(RAD(20)), maxAngle(RAD(90)), minAngle(RAD(-90)), 
  eid(_eid), oldAngle(0), oldAngleSpeed(angleSpeed),
  locked(false)
{}

bool 
Door::step(R dt)
{
  angle+=angleSpeed*dt;
  if (angle>maxAngle) {angle=maxAngle-(angle-maxAngle);angleSpeed*=-1;}
  if (angle<minAngle) {angle=minAngle+(minAngle-angle);angleSpeed*=-1;}
  angleSpeed-=angleSpeed*damping*dt;
  if (angleSpeed>RAD(90)) angleSpeed=RAD(90);
  if (angleSpeed<RAD(-90)) angleSpeed=RAD(-90);
  return true;
}

bool
Door::isClosed() const
{
  return (angle<RAD(5))&&(angle>RAD(-5));
  return true;
}
