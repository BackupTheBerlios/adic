#include "sdlgl.h"

#include <SDL/SDL.h>
#include <assert.h>
#include <iostream>
#include <dope/dope.h>
#include <iomanip>

#ifdef DLOPEN_OPENGL

/* hack since SDL does not get glu symbols
  \todo either remove this completely and try to find a solution within SDL
  or at least add a deinit

  \note this will have portability problems for sure
*/
#include <dlfcn.h>

void* altHandle=NULL;

void*
initAltHandle()
{
  if (altHandle) return altHandle;
  altHandle = dlopen ("/usr/lib/libGLU.so", RTLD_LAZY);
  return altHandle;
}

_GLUfuncptr
altGetProcAddress(char *symbol)
{
  if (!initAltHandle()) return NULL;
  return (_GLUfuncptr)dlsym(altHandle,symbol);
}
// end hack

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

#define FUNC(ret,name,parm) do{typedef ret (*T##name) parm ;name=(T##name)SDL_GL_GetProcAddress(STRINGIFY(name));if(!name) name=(T##name)altGetProcAddress(STRINGIFY(name));assert(name);DEBUG_GL("got address of:" STRINGIFY(name) " it is at: " << ((void *)name));}while(0)

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
