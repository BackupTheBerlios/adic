#include "utils.h"

#include <fstream>
#include <dope/utils.h>

std::string *dataPathPtr=NULL;

std::string findDataFile(const std::string &fname, const char *dataPath)
{
  std::string first,path;
  if (dataPath)
    path=dataPath;
  else if (dataPathPtr)
    path=*dataPathPtr;
  else
    path=ADIC_DATAPATH;
  bool c=true;
  do {
    c=split(path,first,path,':');
    int s=first.size();
    if (s&&(first[s-1]!='/')) first+='/';
    std::string n(first+fname);
#if defined(WINDOOF)
    for (unsigned i=0;i<n.size();++i) if (n[i]=='/') n[i]='\\';
#endif
    std::ifstream ist(n.c_str());
    if (ist.good()) return n;
  }while(c);
  return "";
}
