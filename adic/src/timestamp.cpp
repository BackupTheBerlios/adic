#include "timestamp.h"
#include <sys/time.h>
#include <unistd.h>


void TimeStamp::now()
{
  timeval s;
  gettimeofday(&s,NULL);
  m_sec=s.tv_sec;
  m_usec=s.tv_usec;
}
  
TimeStamp TimeStamp::operator-(const TimeStamp &o) const
{
  // modified version of timeval_subtract from info libc
  // \todo is this really good ?
  timeval x,y,r;
  
  if (x.tv_usec < y.tv_usec) {
    int nsec = (y.tv_usec - x.tv_usec) / 1000000 + 1;
    y.tv_usec -= 1000000 * nsec;
    y.tv_sec += nsec;
  }
  if (x.tv_usec - y.tv_usec > 1000000) {
    int nsec = (x.tv_usec - y.tv_usec) / 1000000;
    y.tv_usec += 1000000 * nsec;
    y.tv_sec -= nsec;
  }
  
  r.tv_sec = x.tv_sec - y.tv_sec;
  r.tv_usec = x.tv_usec - y.tv_usec;
  
  assert(x.tv_sec>=y.tv_sec);
  
  return TimeStamp(r.tv_sec,r.tv_usec);
}


void TimeStamp::sleep() const
{
  timeval s;
  s.tv_sec=m_sec;
  s.tv_usec=m_usec;
  select(0,0,0,0,&s); // todo may be interrupted by signals
}

