#include "glpoly.h"
#include "sdlgl.h"

#ifndef WINAPI
#define DOPE_CVOIDFUNC static void
#else
#define DOPE_CVOIDFUNC static void WINAPI
#endif

#define STRINGIFY(m) #m

extern "C" {

DOPE_CVOIDFUNC myBegin(GLenum i)
{
  //  DOPE_MSG("Info:","entered. DOPE_CVOIDFUNC="<<STRINGIFY(DOPE_CVOIDFUNC)<<std::flush);
  glBegin(i);
  return;
}

DOPE_CVOIDFUNC myVertex(GLvoid *_v)
{
  //  DOPE_MSG("Info:","entered"<<std::flush);
  R* v=(R *)_v;
  //  DOPE_MSG("Info:","v={"<<v[0]<<","<<v[1]<<"}"<<std::flush);
  // todo 64 should not be hardcoded
  glTexCoord2f(v[0]/128, v[1]/128);
  glVertex2f(v[0],v[1]);
  return;
}

DOPE_CVOIDFUNC myEnd(void)
{
  //  DOPE_MSG("Info:","entered"<<std::flush);
  glEnd();
  return;
}
 
}
#undef DOPE_CVOIDFUNC
#undef STRINGIFY

GLPoly::GLPoly(const std::vector<V2D> &poly)
{
  // tesselate polygon 
  typedef GLdouble V3D[3];
  typedef GLvoid (*CallBack)(GLvoid);

  // gluTessVertex takes a pointer to 3 doubles
  // but we have 2 floats => create array suitable for gluTessVertex
  V3D* v=new V3D[poly.size()];
  for (unsigned i=0;i<poly.size();++i) {
    v[i][0]=poly[i][0];
    v[i][1]=poly[i][1];
    v[i][2]=0;
  }
  GLUtesselator* tobj=gluNewTess();
  gluTessCallback(tobj, GLU_TESS_BEGIN, (CallBack)myBegin); 
  gluTessCallback(tobj, GLU_TESS_VERTEX,  (CallBack)myVertex);
  gluTessCallback(tobj,  GLU_TESS_END, (CallBack)myEnd);

  // use the old/obsolete gluBeginPolygon for portability
  //  gluTessBeginPolygon(tobj,NULL);
  gluBeginPolygon(tobj);
  for (unsigned i=0;i<poly.size();++i) {
    gluTessVertex(tobj, v[i], (GLvoid*)(poly[i].m_v));
  }
  l=glGenLists(1);
  DOPE_CHECK(l); // todo
  glNewList(l,GL_COMPILE);
  //  gluTessEndPolygon(tobj);
  //  DOPE_MSG("Info:","reached"<<std::flush);
  gluEndPolygon(tobj);
  //  DOPE_MSG("Info:","reached"<<std::flush);
  glEndList();
  //  DOPE_MSG("Info:","reached"<<std::flush);
  gluDeleteTess(tobj);
  //  DOPE_MSG("Info:","reached"<<std::flush);
  delete [] v;
  //  DOPE_MSG("Info:","reached"<<std::flush);
}

GLPoly::~GLPoly()
{
  glDeleteLists(l,1);
}

void
GLPoly::draw()
{
  glCallList(l);
}

