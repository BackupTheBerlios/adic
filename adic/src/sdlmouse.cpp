#include "sdlmouse.h"

SDLMouse* 
SDLMouse::create(SDLSigFactory &sf, const InputDevName &n)
{
  if (n.minor>0) return NULL;
  return new SDLMouse(sf,n);
}

SDLMouse::SDLMouse(SDLSigFactory &sf, const InputDevName &n) 
  : SDLInputDev(n)
{
  sf.mouseMotion.connect(SigC::slot(*this,&SDLMouse::handleMotion));
  sf.mouseButton.connect(SigC::slot(*this,&SDLMouse::handleButton));
  sf.resize.connect(SigC::slot(*this,&SDLMouse::handleResize));
}

int8_t 
SDLMouse::scale(Uint16 v)
{
  int y=v;
  y-=my;
  int clearance=my>>3;
  if (y>clearance)
    return -1;
  if (y<-clearance)
    return 1;
  return 0;
}

  
void
SDLMouse::handleMotion(SDL_MouseMotionEvent e)
{
  setY(scale(e.y));
}

void
SDLMouse::handleButton(SDL_MouseButtonEvent e)
{
  Input old(m_state);
  unsigned b=e.button;
  bool pressed=e.state==SDL_PRESSED;
  if (b==SDL_BUTTON_LEFT) {
    // left 
    if (pressed) m_state.x=-1;
    else if (m_state.x==-1) m_state.x=0;
  }else if (b==SDL_BUTTON_RIGHT) {
    // right
    if (pressed) m_state.x=1;
    else if (m_state.x==1) m_state.x=0;
  }else if (b==4) {
    // up
    if (pressed) m_state.y=1;
    else if (m_state.y==1) m_state.y=0;
  }else if (b==5) {
    // down
    if (pressed) m_state.y=-1;
    else if (m_state.y==-1) m_state.y=0;
  }
  if (m_state!=old)
    input.emit(m_state);
}

void
SDLMouse::handleResize(SDL_ResizeEvent e)
{
  mx=e.w>>1;
  my=e.h>>1;
}

