#include "door.h"
#include "wall.h"
//#include "world.h"

const R Door::damping=RAD(3);

Door::Door(FWEdge::EID _eid)
  : angle(RAD(45)), angleSpeed(RAD(0)), maxAngle(RAD(90)), minAngle(RAD(-90)), 
  eid(_eid), oldAngle(0), oldAngleSpeed(angleSpeed),
  locked(false)
{}

bool 
Door::step(R dt)
{
  if (angleSpeed==R(0))
    return true;
  int sig1=(angle>0) ? 1 : ((angle<0) ? -1 : 0);
  angle+=angleSpeed*dt;
  int sig2=(angle>0) ? 1 : ((angle<0) ? -1 : 0);
  if ((sig1!=sig2)&&(fabs(angleSpeed)<RAD(45))) {
    angle=0;
    angleSpeed=0;
    return true;
  }else if (fabs(angleSpeed)<RAD(2)) {
    angleSpeed=0;
    return true;
  }
  if (angle>maxAngle) {angle=maxAngle-(angle-maxAngle);angleSpeed*=-1;}
  if (angle<minAngle) {angle=minAngle+(minAngle-angle);angleSpeed*=-1;}
  sig1=(angleSpeed>0) ? 1 : ((angleSpeed<0) ? -1 : 0);
  angleSpeed-=angleSpeed*damping*dt+R(sig1)*damping*dt;
  sig2=(angleSpeed>0) ? 1 : ((angleSpeed<0) ? -1 : 0);
  if (sig1!=sig2) angleSpeed=0;
  else if (angleSpeed>RAD(180)) angleSpeed=RAD(180);
  else if (angleSpeed<RAD(-180)) angleSpeed=RAD(-180);
  return true;
}

bool
Door::isClosed() const
{
  return (angle<RAD(5))&&(angle>RAD(-5));
  return true;
}
