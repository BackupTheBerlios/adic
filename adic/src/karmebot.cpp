#include "karmebot.h"


bool
KarmeBot::step(R dt)
{
  const Player &me(client.getPlayers()[pid]);
  
  cinput.y=1;
  cinput.devno=inputID;
  
  reachDir();
  sendInput();
  return true;
}

void
KarmeBot::playerCollision(PlayerID cp, const V2D &cv)
{
  std::cerr << "I am "<<pid<<" and collided with "<<cp<<"\n";
}

void 
KarmeBot::wallCollision(const std::vector<FWEdge::EID> &eids, const V2D &cv)
{
  std::cerr << "I am "<<pid<<" and collided with wall(s):";
  for (unsigned w=0;w<eids.size();++w){
    std::cerr << " "<<eids[w];
  }
  std::cerr << "\n";
  V2D ndir(cv);
  ndir.rot90();
  
  dir=vdir(ndir);
  while (dir<0) dir+=(M_PI*2);
  std::cerr << "cv: "<<cv<<" ndir:"<<ndir<<" => dir:"<<dir<<"\n";
}

void
KarmeBot::doorCollision(unsigned did, const V2D &cv)
{
  std::cerr << "I am "<<pid<<" and collided with door: "<<did<<"\n";
  V2D ndir(-cv);
  dir=vdir(ndir);
  while (dir<0) dir+=(M_PI*2);
}
  

void
KarmeBot::reachDir()
{
  const Player &me(client.getPlayers()[pid]);
  // get current direction
  R cdir=me.getDirection();
  R diff=cdir-dir;
  R threshold=10;
  
  while (diff>M_PI) diff-=(M_PI*2);
  while (diff<-M_PI) diff+=(M_PI*2);
  if (diff<-M_PI/180*threshold) cinput.x=-1;
  else if (diff>M_PI/180*threshold) cinput.x=+1;
  else cinput.x=0;
}
