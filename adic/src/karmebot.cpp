#include "karmebot.h"

//! a single karme bot (=one player)
class BotPlayer
{
public:
  BotPlayer(BotClient &_client, PlayerID _pid, unsigned _inputID) 
    : client(_client), pid(_pid), inputID(_inputID)
  {}

  int step(R dt)
  {
    const Player &me(client.getPlayers()[pid]);

    cinput.y=1;
    cinput.devno=inputID;
    
    reachDir();
    
    // emit input signal if we changed our controls
    if (cinput!=oinput) {
      input.emit(cinput);
      oinput=cinput;
    }
    return true;
  }

  SigC::Signal1<void, Input &> input;

  void playerCollision(PlayerID cp, const V2D &cv)
  {
    std::cerr << "I am "<<pid<<" and collided with "<<cp<<"\n";
  }
  void wallCollision(const std::vector<FWEdge::EID> &eids, const V2D &cv)
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
  void doorCollision(unsigned did, const V2D &cv)
  {
    std::cerr << "I am "<<pid<<" and collided with door: "<<did<<"\n";
    V2D ndir(-cv);
    if (ndir[1]!=0.0)
      dir=atan(ndir[0]/ndir[1]);
    else
      dir=(ndir[0]>0) ? M_PI/2 : -M_PI/2;
  }
  

  //! try to rotate to wished direction - by setting cinput as needed
  void reachDir()
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
protected:  
  BotClient& client;
  PlayerID pid;
  unsigned inputID;
  Input cinput;
  Input oinput;

  //! wished direction
  R dir;

  //! calculates direction of given vector
  R vdir(const V2D &v)
  {
    // our coord sys is a little bit wired (we have clockwise angles and start from 0/1)
    return M_PI/2-atan2(v[1],v[0]);
  }
};

std::vector<DOPE_SMARTPTR<BotPlayer> > bots;

KarmeBot::KarmeBot(BotClient &client)
  : Bot(client)
{
  // we want 4 players all in the Kabot team
  // Note: we do not get 4 players for sure !
  BotClientConfig &config(getConfig());
  config.m_users.users.clear();
  config.m_users.users.push_back(User("Rob","Kabot"));
  config.m_users.users.push_back(User("Tinky","Kabot"));
  config.m_users.users.push_back(User("Bob","Kabot"));
  config.m_users.users.push_back(User("Lobster","Kabot"));
}

bool
KarmeBot::step(R dt)
{
  for (unsigned i=0;i<bots.size();++i)
    bots[i]->step(dt);
  return true;
}

void
KarmeBot::startGame()
{
  bots.clear();
  const std::vector<PlayerID> &pids(client.getMyIDs());
  for (unsigned p=0;p<pids.size();++p){
    bots.push_back(DOPE_SMARTPTR<BotPlayer>(new BotPlayer(client,pids[p],p)));
    bots[p]->input.connect(input.slot());
  }
}


// dispatch events to correct PlayerBot

unsigned
KarmeBot::getPlayerBotID(PlayerID pid)
{
  const std::vector<PlayerID> &myids(client.getMyIDs());
  assert(myids.size()==bots.size());
  for (unsigned i=0;i<myids.size();++i) {
    if (myids[i]==pid) return i;
  }
  return ~0U;
}


void
KarmeBot::handlePlayerCollision(PlayerID p1, PlayerID p2, const V2D &cv)
{
  unsigned botid=getPlayerBotID(p1);
  if (botid!=~0U) {
    bots[botid]->playerCollision(p2,cv);
    return;
  }
  //  else
  botid=getPlayerBotID(p2);
  if (botid!=~0U) {
    bots[botid]->playerCollision(p1,cv);
    return;
  }
}

void
KarmeBot::handleWallCollision(PlayerID p, const std::vector<FWEdge::EID> &eids, const V2D &cv)
{
  unsigned botid=getPlayerBotID(p);
  if (botid==~0U) return;
  bots[botid]->wallCollision(eids,cv);
}

void
KarmeBot::handleDoorCollision(PlayerID p, unsigned did, const V2D &cv)
{
  unsigned botid=getPlayerBotID(p);
  if (botid==~0U) return;
  bots[botid]->doorCollision(did,cv);
}

