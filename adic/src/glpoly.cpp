#include "glpoly.h"
#include "sdlgl.h"

void myBegin(GLenum i)
{
  glBegin(i);
}

void myVertex3dv(GLdouble *v)
{
  // todo 64 should not be hardcoded
  glTexCoord2f(v[0]/128, v[1]/128);
  glVertex3dv(v);
}

void myEnd()
{
  glEnd();
}

GLPoly::GLPoly(const std::vector<V2D> &poly)
{
  // tesselate polygon 
  typedef GLdouble V3D[3];
  V3D* v=new V3D[poly.size()];
  for (unsigned i=0;i<poly.size();++i) {
    v[i][0]=poly[i][0];
    v[i][1]=poly[i][1];
    v[i][2]=0;
  }
  GLUtesselator* tobj=gluNewTess();
  gluTessCallback(tobj, GLU_TESS_BEGIN, (_GLUfuncptr)myBegin); 
  gluTessCallback(tobj, GLU_TESS_VERTEX,  (_GLUfuncptr)myVertex3dv);
  gluTessCallback(tobj,  GLU_TESS_END, (_GLUfuncptr)myEnd);
  gluTessBeginPolygon(tobj,NULL);
  for (unsigned i=0;i<poly.size();++i) {
    gluTessVertex(tobj, v[i], v[i]);
  }
  l=glGenLists(1);
  glNewList(l,GL_COMPILE);
  //  glBindTexture(GL_TEXTURE_2D, texture[0]);   // choose the texture to use.
  gluTessEndPolygon(tobj);
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

