#include "karmebot.h"


R threshold=5;

KarmeBot::KarmeBot(BotClient &_client, PlayerID _pid, unsigned _inputID) 
  : Bot(_client,_pid,_inputID), followMode(false)
{
  //  std::cerr << "sizeof(input)"<< sizeof(cinput) << "\n";
}


bool
KarmeBot::step(R dt)
{
  const Player &me(client.getPlayers()[pid]);
  
  cinput.y=1;
  cinput.devno=inputID;
  
  follow();
  reachDir();
  sendInput();
  return true;
}

void
KarmeBot::playerCollision(PlayerID cp, const V2D &cv)
{
  std::cerr << "I am "<<pid<<" and collided with "<<cp<<"\n";
  const Player &me(client.getPlayers()[pid]);
  const Player &other(client.getPlayers()[cp]);
  if (!other.isPlayer())
    return;
  if (cp>pid) {
    const std::vector<PlayerID> &myIDs(client.getMyIDs());
    for (unsigned i=0;i<myIDs.size();++i)
      if (myIDs[i]==cp) return;
  }
  rabbit=cp;
  followMode=true;
}

void 
KarmeBot::wallCollision(const std::vector<FWEdge::EID> &eids, const V2D &cv)
{
  std::cerr << "I am "<<pid<<" and collided with wall(s):";
  for (unsigned w=0;w<eids.size();++w){
    std::cerr << " "<<eids[w];
  }
  std::cerr << "\n";
  // it seems cv is exactly int the wrong direction we should have to use +cv here
  dir=vdir(-cv.rot(M_PI/180*(threshold+90)));
  while (dir<0) dir+=(M_PI*2);
  followMode=false;
}

void
KarmeBot::doorCollision(unsigned did, const V2D &cv)
{
  std::cerr << "I am "<<pid<<" and collided with door: "<<did<<"\n";
  // it seems cv is exactly int the wrong direction we should have to use -cv here
  dir=vdir(cv);
  while (dir<0) dir+=(M_PI*2);
  followMode=false;
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
  if (!followMode)
    return;
  const Player &me(client.getPlayers()[pid]);
  const Player &other(client.getPlayers()[rabbit]);
  V2D diff(other.m_pos-me.m_pos);
  dir=vdir(diff);
  while (dir<0) dir+=(M_PI*2);
}

