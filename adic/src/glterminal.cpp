#include "glterminal.h"
#include "sdlglgui.h"
#include "sdlgl.h"
#include <cstdlib>

GLTerminal::GLTerminal
(SDLGLGUI &_gui, int _x, int _y, bool _centered, R _scrollspeed, R _writespeed)
  : gui(_gui), gl(_gui.gl), x(_x), y(_y), centered(_centered),
    stimer(_scrollspeed), wtimer(0), 
    scrollspeed(_scrollspeed),
    writespeed(_writespeed)
{}

void
GLTerminal::appendRow(std::string r)
{
  while (r.size()>maxcols) {
    buffered.push_back(std::string(r,0,maxcols));
    r=r.substr(maxcols);
  }
  buffered.push_back(r);
}

void
GLTerminal::append(const std::string &text)
{
  std::string::size_type pos(text.find_first_of('\n'));
  if (pos!=std::string::npos)
    appendRow(std::string(text,0,pos));
  if (pos+1<text.size()) {
    append(std::string(text,pos+1));
  }
}

void
GLTerminal::step(R dt)
{
  const std::string &newtext(out.str());
  if (newtext.size()) {
    int view[4];
    gl.GetIntegerv(GL_VIEWPORT,view);
    int w=view[2];
    int h=view[3];
    maxcols=(w-x)/gui.m_fontPtr->getWidth();
    maxrows=y/gui.m_fontPtr->getHeight();
    append(newtext);
    out.str("");
    while (rows.size()>maxrows) {
      rows.pop_front();
    }
  }
  if (rows.size()) {
    if ((buffered.empty())||(rows.size()>2))
      stimer-=dt;
    if (stimer<0) {
      stimer+=scrollspeed;
      rows.pop_front();
      if (rows.empty()) stimer=scrollspeed;
    }
  }
  if (buffered.size()) {
    std::string &src(buffered.front());
    wtimer+=dt*writespeed;
    unsigned chars=0;
    if (wtimer>0) chars=wtimer;
    if (chars) {
      wtimer-=chars;
#define min(a,b) (a<=b) ? a : b
      chars=min(chars,src.size());
#undef min
      if (!chars) {
	  buffered.pop_front();
	  rows.push_back();
	  printed.emit('\n');
	  wtimer-=R(maxcols)/writespeed;
      }else{
	if (rows.empty()) rows.push_back();
	std::string &dest(rows.back());
	std::string w(src.substr(0,chars));
	printed.emit(w[0]);
	wtimer-=R(rand()/(RAND_MAX/100))*0.01*30/writespeed;
	dest+=w;
	if (chars<src.size())
	  src=src.substr(chars);
	else
	  {
	    buffered.pop_front();
	    rows.push_back();
	    printed.emit('\n');
	    wtimer-=R(maxcols)/writespeed;
	  }
      }
    }
  }
  if (rows.empty())
    return;
  gl.PushMatrix();
  gl.LoadIdentity();
  gl.Translatef(x,y-int(gui.m_fontPtr->getHeight()*stimer/scrollspeed),0);
  std::list<std::string>::iterator it(rows.begin());
  while (it!=rows.end()) {
    gui.m_fontPtr->drawTextRow(*it,centered);
    gl.Translatef(0,-gui.m_fontPtr->getHeight(),0);
    ++it;
  }
  gl.PopMatrix();
}
