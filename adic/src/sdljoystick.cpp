#include "sdljoystick.h"

SDLJoystick* 
SDLJoystick::create(SDLSigFactory &sf, const InputDevName &n)
{
  int numj=SDL_NumJoysticks();
  //  std::cerr << "Found "<<numj<< " joysticks\n";
  if (n.minor>=numj)
    return NULL;
  SDL_Joystick* joy=SDL_JoystickOpen(n.minor);
  if (!joy) return NULL;
  if (SDL_JoystickNumAxes(joy)<2) {
    SDL_JoystickClose(joy);
    return NULL;
  }
  return new SDLJoystick(sf,n,joy);
}


SDLJoystick::SDLJoystick(SDLSigFactory &sf, const InputDevName &n, SDL_Joystick *j) 
  : SDLInputDev(n), m_joy(j)
{
  sf.joyMotion.connect(SigC::slot(*this,&SDLJoystick::handleEvent));
}

int8_t
SDLJoystick::scale(Sint16 v) 
{
  Sint16 clearance=20000;
  if (v>clearance) return 1;
  if (v<-clearance) return -1;
  return 0;
}

void
SDLJoystick::handleEvent(SDL_JoyAxisEvent event)
{
  if (event.which!=m_devName.minor)
    return;
  switch (event.axis) {
  case 0: 
    setX(scale(event.value));
    return;
  case 1:
    setY(scale(-event.value));
    return;
  }
}
