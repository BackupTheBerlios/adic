#include "sdlgl.h"

#include <SDL/SDL.h>
#include <assert.h>

SDLGL::SDLGL()
{
}

void
SDLGL::init()
{
#define STRINGIFY(m) #m
#define LOOKUP(m,t) m=(t)SDL_GL_GetProcAddress(STRINGIFY(gl##m));assert(m)
  LOOKUP(Clear,uintFunc);
  LOOKUP(MatrixMode,uintFunc);
  LOOKUP(LoadIdentity,voidFunc);
  LOOKUP(Color3f,fvec3Func);
  LOOKUP(Color4f,fvec4Func);
  LOOKUP(Translatef,fvec3Func);
  LOOKUP(Scalef,fvec3Func);
  LOOKUP(Begin,uintFunc);
  LOOKUP(Vertex2i,ivec2Func);
  LOOKUP(Vertex2f,fvec2Func);
  LOOKUP(End,voidFunc);
  LOOKUP(Viewport,ivec4Func);
  LOOKUP(Ortho,dvec6Func);
  LOOKUP(ClearColor,fvec4Func);
  LOOKUP(PushMatrix,voidFunc);
  LOOKUP(PopMatrix,voidFunc);
  LOOKUP(GetFloatv,uintfloatPFunc);
  LOOKUP(GetIntegerv,uintintPFunc);
  LOOKUP(LineWidth,floatFunc);
  LOOKUP(Flush,voidFunc);
  LOOKUP(Finish,voidFunc);
  LOOKUP(Enable,uintFunc);
  LOOKUP(Disable,uintFunc);
  LOOKUP(BlendFunc,uint2Func);
  LOOKUP(TexCoord2f,fvec2Func);
  LOOKUP(BindTexture,uint2Func);
  LOOKUP(GenTextures,uintuintPFunc);
  LOOKUP(TexParameteri,uint2intFunc);
  LOOKUP(TexImage2D,glTexImage2DFunc);
  LOOKUP(Rotatef,fvec4Func);
#undef LOOKUP
}
