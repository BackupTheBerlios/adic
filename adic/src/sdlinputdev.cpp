#include "sdlinputdev.h"
#include "sdlkeyboard.h"
#include "sdljoystick.h"
#include "sdlmouse.h"

SDLInputDev* 
SDLInputDev::create(SDLSigFactory &sf, const InputDevName &n)
{
  switch(n.major) {
  case 0:
    return SDLMouse::create(sf,n);
  case 1:
    return SDLKeyboard::create(sf,n);
  case 2:
    return SDLJoystick::create(sf,n);
  }
  return NULL;
}


