#include "glterminal.h"
#include "glfont.h"
#include "sdlgl.h" // to get the OpenGL function declarations (no dependency on SDL)
#include <cstdlib>

GLTerminal::GLTerminal
(const GLFont &font, int _x, int _y, bool _centered, R _scrollspeed, R _writespeed)
  : m_font(font), x(_x), y(_y), centered(_centered),
    stimer(_scrollspeed), wtimer(0), 
    scrollspeed(_scrollspeed),
    writespeed(_writespeed)
{}

void
GLTerminal::appendRow(String r)
{
  while (r.size()>maxcols) {
    buffered.push_back(String(r,0,maxcols));
    r=r.substr(maxcols);
  }
  buffered.push_back(r);
}

void
GLTerminal::append(const String &text)
{
  String::size_type pos(text.find_first_of('\n')); // todo '\n'
  if (pos!=String::npos)
    appendRow(String(text,0,pos));
  if (pos+1<text.size()) {
    append(String(text,pos+1));
  }
}


void
GLTerminal::print(const std::string &newText)
{
  if (!newText.size()) return;
  int view[4];
  glGetIntegerv(GL_VIEWPORT,view);
  int w=view[2];
  int h=view[3];
  maxcols=(w-x)/m_font.getWidth();
  maxrows=y/m_font.getHeight();
  append(newText);
  while (rows.size()>maxrows) {
    rows.pop_front();
  }
}


void
GLTerminal::step(R dt)
{
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
    String &src(buffered.front());
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
	  printed.emit('\n'); // todo \n
	  wtimer-=R(maxcols)/writespeed;
      }else{
	if (rows.empty()) rows.push_back();
	String &dest(rows.back());
	String w(src.substr(0,chars));
	printed.emit(w[0]);
	wtimer-=R(rand()/(RAND_MAX/100))*0.01*30/writespeed;
	dest+=w;
	if (chars<src.size())
	  src=src.substr(chars);
	else
	  {
	    buffered.pop_front();
	    rows.push_back();
	    printed.emit('\n'); // todo \n
	    wtimer-=R(maxcols)/writespeed;
	  }
      }
    }
  }
  if (rows.empty())
    return;
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(x,y-int(m_font.getHeight()*stimer/scrollspeed),0);
  std::list<String>::iterator it(rows.begin());
  while (it!=rows.end()) {
    m_font.drawTextRow(*it,centered);
    glTranslatef(0,-m_font.getHeight(),0);
    ++it;
  }
  glPopMatrix();
}
