#include "blackbot.h"


static R threshold=5;

BlackBot::BlackBot(BotClient &_client, PlayerID _pid, unsigned _inputID) 
  : Bot(_client,_pid,_inputID), followMode(false)
{
}


bool
BlackBot::step(R dt)
{
  const Player &me(client.getPlayers()[pid]);
  // walk forward
  cinput.y=1;
  cinput.devno=inputID;
  
  if (followMode) follow();
  reachDir();
  sendInput();
  return true;
}

void
BlackBot::playerCollision(PlayerID cp, const V2D &cv)
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
  followMode=true;
}

void 
BlackBot::wallCollision(const std::vector<FWEdge::EID> &eids, const V2D &cv)
{
  /*  std::cerr << "I am "<<pid<<" and collided with wall(s):";
      for (unsigned w=0;w<eids.size();++w){
      std::cerr << " "<<eids[w];
      }
      std::cerr << "\n";
  */
  dir=vdir(cv.rot(M_PI/180*(threshold+90)));
  followMode=false;
}

void
BlackBot::doorCollision(unsigned did, const V2D &cv)
{
  //  std::cerr << "I am "<<pid<<" and collided with door: "<<did<<"\n";
  dir=vdir(-cv);
  followMode=false;
}
  

void
BlackBot::reachDir()
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
BlackBot::follow()
{
  const Player &me(client.getPlayers()[pid]);
  const Player &other(client.getPlayers()[rabbit]);
  V2D diff(other.m_pos-me.m_pos);
  dir=vdir(diff);
}

