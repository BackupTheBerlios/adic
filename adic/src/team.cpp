#include "team.h"
#include <dope/utils.h>

Team::Team(const std::string &_name,unsigned tno) 
  : name(_name)
{
  std::string colorname;
  switch(tno%4) {
  case 0:
    colorname="_green_000";
    color[0]=0.1;
    color[1]=1.0;
    color[2]=0.1;
    break;
  case 1:
    colorname="_red_000";
    color[0]=1.0;
    color[1]=0.1;
    color[2]=0.1;
    break;
  case 2:
    colorname="_yellow_000";
    color[0]=1.0;
    color[1]=0.9;
    color[2]=0.1;
    break;
  case 3:
    colorname="_blue_000";
    color[0]=0.0;
    color[1]=0.5;
    color[2]=1.0;
    break;
  }
  textures.resize(4);
  dataURIs.resize(4);
  for (unsigned i=0;i<textures.size();++i) {
    std::string mdl;
    switch(i%textures.size()) {
    case 0:
      mdl="boy";
      break;
    case 1:
      mdl="girl";
      break;
    case 2:
      mdl="muetze";
      break;
    case 3:
      mdl="cowboy";
      break;
    }
    dataURIs[i]=std::string("data:")+mdl+".xml";
    for (unsigned f=1;f<7;++f) {
      textures[i].push_back(std::string("data:")+mdl+colorname+anyToString(f)+".png");
    }
  }
}
