#include "sdlgl.h"

#include <SDL/SDL.h>
#include <assert.h>
#include <iostream>
#include <dope/dope.h>
#include <iomanip>

#ifdef DLOPEN_OPENGL

#define FUNC(ret,name,parm) ret (* name ) parm=0;
#include "glfunctions.h"
#undef FUNC

#endif


SDLGL::SDLGL()
{
}

void
SDLGL::init()
{
#define STRINGIFY(m) #m

#ifdef DLOPEN_OPENGL
  DEBUG_GL("looking up symbols with SDL_GL_GetProcAddress");
  DEBUG_GL("init called => we loaded the dll and now lookup the symbols");

#define FUNC(ret,name,parm) do{typedef ret (*T##name) parm ;name=(T##name)SDL_GL_GetProcAddress(STRINGIFY(name));assert(name);DEBUG_GL("got address of:" STRINGIFY(name) " it is at: " << ((void *)name));}while(0)

#include "glfunctions.h"

#undef FUNC

#endif
}

int
SDLGL::printErrors()
{
  int res=0;
  int e;
  while ((e=glGetError())) {
    DOPE_MSG("GL Error: ", e);
    std::ios::fmtflags old=std::cerr.flags();
    std::cerr << "GL Error: " << e << "=0x" << std::hex << e << std::endl;
    std::cerr.flags(old);
    ++res;
    if (res>5) {
      DOPE_WARN("stop printing errors because there are too much (parhaps some other problem)");
      return res;
    }
  }
  return res;
}
