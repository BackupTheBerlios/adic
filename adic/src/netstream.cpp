#include "netstream.h"

NetStream::NetStream(const std::string &name, unsigned short int port)
  : adr(HostAddress(name.c_str()),port),
    layer0(adr), l2out(layer0), 
#if USE_RAW_PROTOCOL == 1
    l2in(layer0), 
#elif USE_XML_PROTOCOL == 1
    l2in(layer0,TimeStamp(0,300),2),
#endif
    so(l2out), si(l2in)
{
}
