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
  int sig1=(angleSpeed>0) ? 1 : ((angleSpeed<0) ? -1 : 0);
  angleSpeed-=angleSpeed*damping*dt+R(sig1)*damping*dt;
  int sig2=(angleSpeed>0) ? 1 : ((angleSpeed<0) ? -1 : 0);
  if (sig1!=sig2) angleSpeed=0;
  else if (angleSpeed>RAD(90)) angleSpeed=RAD(90);
  else if (angleSpeed<RAD(-90)) angleSpeed=RAD(-90);
  return true;
}

bool
Door::isClosed() const
{
  return (angle<RAD(5))&&(angle>RAD(-5));
  return true;
}
