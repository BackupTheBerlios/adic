#include "sdlkeyboard.h"

SDLKeyboard* 
SDLKeyboard::create(SDLSigFactory &sf, const InputDevName &n)
{
  if (n.minor>1) return NULL;
  return new SDLKeyboard(sf,n);
}

SDLKeyboard::Keys
SDLKeyboard::keys[2]={
  {SDLK_KP8, SDLK_KP2, SDLK_KP4, SDLK_KP6},
  {SDLK_w, SDLK_s, SDLK_a, SDLK_d}
};


SDLKeyboard::SDLKeyboard(SDLSigFactory &sf, const InputDevName &n) 
  : SDLInputDev(n)
{
  sf.keyEvent.connect(SigC::slot(*this,&SDLKeyboard::handleKey));
}
  
bool
SDLKeyboard::handleKey(SDL_KeyboardEvent e)
{
  Input old(m_state);
  bool pressed=e.state==SDL_PRESSED;
  SDLKey k(e.keysym.sym);
  if (k==keys[m_devName.minor][0]) {
    // up
    if (pressed) m_state.y=1;
    else if (m_state.y==1) m_state.y=0;
  }else if (k==keys[m_devName.minor][1]) {
    // down
    if (pressed) m_state.y=-1;
    else if (m_state.y==-1) m_state.y=0;
  }else if (k==keys[m_devName.minor][2]) {
    // left 
    if (pressed) m_state.x=-1;
    else if (m_state.x==-1) m_state.x=0;
  }else if (k==keys[m_devName.minor][3]) {
    // right
    if (pressed) m_state.x=1;
    else if (m_state.x==1) m_state.x=0;
  }
  if (m_state!=old) {
    input.emit(m_state);
    return true;
  }
  return false;
}
