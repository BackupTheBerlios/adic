#include "karmebot.h"

const R KarmeBot::threshold=5;

KarmeBot::KarmeBot(BotClient &_client, PlayerID _pid, unsigned _inputID) 
  : Bot(_client,_pid,_inputID), mode(DEFAULT)
{
}


bool
KarmeBot::step(R dt)
{
  const Player &me(client.getPlayers()[pid]);
  // walk forward
  cinput.y=1;
  cinput.devno=inputID;
  
  switch (mode) {
  case FOLLOW:
    follow();
    break;
  case CROSSLINE:
    crossLine();
    break;
  default:
    break;
  }
  
  reachDir();
  sendInput();
  return true;
}

void
KarmeBot::playerCollision(PlayerID cp, const V2D &cv)
{
  //  std::cerr << "I am "<<pid<<" and collided with "<<cp<<"\n";
  const Player &me(client.getPlayers()[pid]);
  const Player &other(client.getPlayers()[cp]);
  if (!other.isPlayer())
    // this is a spring or barrel
    return;
  if (cp>pid) {
    // dont follow each other - the one with the bigger id will not follow
    const std::vector<PlayerID> &myIDs(client.getMyIDs());
    for (unsigned i=0;i<myIDs.size();++i)
      if (myIDs[i]==cp) return;
  }
  rabbit=cp;
  mode=FOLLOW;
}

void 
KarmeBot::wallCollision(const std::vector<FWEdge::EID> &eids, const V2D &cv)
{
  assert(eids.size());
  followEdge(eids[0]);
}

void
KarmeBot::doorCollision(unsigned did, const V2D &cv)
{
}
  

void
KarmeBot::reachDir()
{
  const Player &me(client.getPlayers()[pid]);
  // get current direction
  R cdir=me.getDirection();
  R diff=dir-cdir;
  R pi=M_PI;

  while (diff>pi) diff-=(pi*2);
  while (diff<-pi) diff+=(pi*2);
  if (diff<-pi/180*threshold) cinput.x=-1;
  else if (diff>pi/180*threshold) cinput.x=+1;
  else cinput.x=0;
}

void
KarmeBot::follow()
{
  const Player &me(client.getPlayers()[pid]);
  const Player &other(client.getPlayers()[rabbit]);
  V2D diff(other.m_pos-me.m_pos);
  dir=vdir(diff);
}

void
KarmeBot::crossLine()
{
  const Player &me(client.getPlayers()[pid]);
  if (!inFront(finishingLine,me.m_pos)) {
    std::cerr << "Crossed Line\n";
    followEdge(nextEdge);
  }
}

void
KarmeBot::followEdge(FWEdge::EID eid)
{
  assert(client.getWorldPtr().get());
  assert(eid!=FWEdge::noEdge);
  const Game::WorldPtr &worldPtr(client.getWorldPtr());
  const FWEdge &e(worldPtr->getEdge(eid));
  const Player &me(client.getPlayers()[pid]);
  mode=CROSSLINE;
  // first we have to know if we are on the clockwise side of the edge
  // which is the case if we are in the same room as the room on the clockwise
  // side of the edge
  FWEdge::RoomID r=client.getGame().playerInRoomCached(pid);
  bool cw=r==e.m_rcw;
  if ((!cw)&&(r!=e.m_rccw)) DOPE_WARN(r<<eid);

  if (!e.isDoor()) {
    Line l(e.m_wall.getLine());
    if (cw) {
      nextEdge=e.m_ncw;
      assert(nextEdge!=FWEdge::noEdge);
    }else{
      nextEdge=e.m_nccw;
      assert(nextEdge!=FWEdge::noEdge);
      l=Line(l.m_b,l.m_a);
    }
    const FWEdge &e2(worldPtr->getEdge(nextEdge));

    Line l2(e2.m_wall.getLine());
    if (r!=e2.m_rcw) l2=Line(l2.m_b,l2.m_a);
    V2D d(l.m_n+l2.m_n);
    finishingLine=Line(l.m_b,l.m_b+d);
    if (!inFront(finishingLine,me.m_pos))
      DOPE_WARN("not in front");
    dir=vdir(l.m_n.rot(-M_PI/180*(90-threshold)));
  }else{
    // door
    Line l(e.m_wall.getLine());
    if (cw) {
      nextEdge=e.m_nccw;
      assert(nextEdge!=FWEdge::noEdge);
    }else{
      nextEdge=e.m_ncw;
      assert(nextEdge!=FWEdge::noEdge);
      l=Line(l.m_b,l.m_a);
    }
    V2D s(l.m_a-l.m_n*l.m_v.norm2());
    finishingLine=Line(s,s+l.m_v);
    if (!inFront(finishingLine,me.m_pos))
      DOPE_WARN("not in front");
    dir=vdir(-l.m_n);
  }
}



