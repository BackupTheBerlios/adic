#include "utils.h"

#include <fstream>
#include <dope/utils.h>

std::string findDataFile(const std::string &fname, const char *dataPath)
{
  std::string first,path;
  if (dataPath)
    path=dataPath;
  else
    path=DATADIR;
  bool c=true;
  do {
    c=split(path,first,path,':');
    int s=first.size();
    if (s&&(first[s-1]!='/')) first+='/';
    std::string n(first+fname);
    std::ifstream i(n.c_str());
    if (i.good())
      return n;
  }while(c);
  return "";
}
