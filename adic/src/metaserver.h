#ifndef METASERVER_H
#define METASERVER_H

#include "typedefs.h"
#include <dope/minihttp.h>

#define IO_CLASS(x) \
DOPE_CLASS(x);\
template<typename Layer2> \
inline void composite(Layer2 &layer2, x &o) \
{ \
o.composite(layer2); \
}

#define IO \
template <typename Layer2> \
void composite(Layer2 &l2) 

struct Host
{
  std::string adr;
  uint16_t port;
  IO {l2.SIMPLE(adr).SIMPLE(port);}
};
IO_CLASS(Host)

//! message a server sends to the meta-server on startup
struct RegisterServer
{
  Host host;
  IO {l2.SIMPLE(host);}
};
IO_CLASS(RegisterServer)

//! meta-server answer to RegisterServer
struct ServerRegistered
{
  bool registered;
  std::string secret;
  IO 
  {
    l2.SIMPLE(registered).SIMPLE(secret);
  }
};
IO_CLASS(ServerRegistered)


//! current server status
struct ServerStatus
{
  ServerStatus() 
    : dopeVersion(dope_major_version,dope_minor_version,dope_micro_version),
      adicVersion(PACKAGE_MAJOR_VERSION,PACKAGE_MINOR_VERSION,PACKAGE_MICRO_VERSION),
      clients(0),
      players(0),
      full(false)
  {}

  Host host;
  Version dopeVersion;
  Version adicVersion;
  uint16_t clients;
  uint16_t players;
  std::string level;
  bool full;

  IO
  {
    l2.SIMPLE(host).SIMPLE(dopeVersion).SIMPLE(adicVersion)
      .SIMPLE(clients).SIMPLE(players).SIMPLE(level).SIMPLE(full);
  }
};
IO_CLASS(ServerStatus)

//! message a server sends to the meta-server to update its status
struct UpdateStatus
{
  std::string secret;  
  ServerStatus status;
  IO
  {
    l2.SIMPLE(secret).SIMPLE(status);
  }
};
IO_CLASS(UpdateStatus)


//! message server sends to meta-server on exit
struct ServerExit
{
  Host host;
  std::string secret;
  
  IO
  {
    l2.SIMPLE(host).SIMPLE(secret);
  }
};
IO_CLASS(ServerExit)

struct Result
{
  int status;
  std::string message;

  void print() 
  {
    std::cerr << "\nMetaserver RPC result: " << status << "=" << message << std::endl;
  }
  
  IO
  {
    l2.SIMPLE(status).SIMPLE(message);
  }
};
IO_CLASS(Result)

//! server list that may be requested from the meta-server
typedef std::list<ServerStatus> ServerList;

struct MetaServer
{
  typedef URLEncodeStream<HTTPStreamBuf> OutProto;
  typedef XMLSAXInStream<HTTPStreamBuf> InProto;

  MetaServer(const char *uristring) : uri(uristring), layer0(uri)
  {}
  
  template <typename X>
  //! send message to metaserver
  /*!
    \param dest the relative path
  */
  void send(X &x)
  {
    OutProto out(layer0);
    const char *funcName=TypeNameTrait<X>::name().c_str();
    out.simple(x,funcName);
    out.flush();
  }

  template <typename X>
  void receive(X &x)
  {
    InProto in(layer0);
    in.simple(x,NULL);
  }
  
  template <typename In, typename Out>
  void rpc(In &in, Out &out) 
  {
    send(in);
    receive(out);
  }
protected:
  URI uri;
  HTTPStreamBuf layer0;
};


#undef IO_CLASS
#undef IO

#endif
