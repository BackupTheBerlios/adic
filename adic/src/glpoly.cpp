#include "glpoly.h"
#include "sdlgl.h"

#ifndef WINAPI
#define WINAPI
#endif

extern "C" {
  
void WINAPI myBegin(GLenum i)
{
  glBegin(i);
}

void WINAPI myVertex3dv(GLdouble *v)
{
  // todo 64 should not be hardcoded
  glTexCoord2f(v[0]/128, v[1]/128);
  glVertex3dv(v);
}

void WINAPI myEnd()
{
  glEnd();
}
 
}


GLPoly::GLPoly(const std::vector<V2D> &poly)
{
  // tesselate polygon 
  typedef GLdouble V3D[3];
  typedef GLvoid (*CallBack)(GLvoid);

  V3D* v=new V3D[poly.size()];
  for (unsigned i=0;i<poly.size();++i) {
    v[i][0]=poly[i][0];
    v[i][1]=poly[i][1];
    v[i][2]=0;
  }
  GLUtesselator* tobj=gluNewTess();
  gluTessCallback(tobj, GLU_TESS_BEGIN, (CallBack)myBegin); 
  gluTessCallback(tobj, GLU_TESS_VERTEX,  (CallBack)myVertex3dv);
  gluTessCallback(tobj,  GLU_TESS_END, (CallBack)myEnd);

  // use the old/obsolete gluBeginPolygon for portability
  //  gluTessBeginPolygon(tobj,NULL);
  gluBeginPolygon(tobj);
  for (unsigned i=0;i<poly.size();++i) {
    gluTessVertex(tobj, v[i], v[i]);
  }
  l=glGenLists(1);
  glNewList(l,GL_COMPILE);
  //  gluTessEndPolygon(tobj);
  gluEndPolygon(tobj);
  glEndList();
  gluDeleteTess(tobj);
  delete [] v;
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

