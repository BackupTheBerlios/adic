#include "sdlglgui.h"

#include <SDL/SDL.h>
#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>	// Header File For The OpenGL32 Library
#include <OpenGL/glu.h>	// Header File For The GLu32 Library
#else
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
#endif

#define USE_DLOPEN

#ifdef USE_DLOPEN
#define LOOKUP(m,t) m##P=(t)SDL_GL_GetProcAddress(#m);DOPE_CHECK(m##P)
#else
#define LOOKUP(m,t) m##P=&m;DOPE_CHECK(m##P)
#endif


SDLGLGUI::SDLGLGUI(Client &client, const GUIConfig &config) 
  : GUI(client,config), m_textureTime(5)
{}


bool
SDLGLGUI::init()
{
  // Initialize SDL
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    throw std::runtime_error(std::string("Couldn't init SDL: ")+SDL_GetError());
  if (SDL_GL_LoadLibrary(m_config.libGL.c_str())==-1)
    throw std::runtime_error(std::string("Could not load OpenGL lib: \"")+m_config.libGL.c_str()+"\": "+SDL_GetError());
  LOOKUP(glClear,uintFunc);
  LOOKUP(glMatrixMode,uintFunc);
  LOOKUP(glLoadIdentity,voidFunc);
  LOOKUP(glColor3f,fvec3Func);
  LOOKUP(glColor4f,fvec4Func);
  LOOKUP(glTranslatef,fvec3Func);
  LOOKUP(glBegin,uintFunc);
  LOOKUP(glVertex2f,fvec2Func);
  LOOKUP(glEnd,voidFunc);
  LOOKUP(glViewport,ivec4Func);
  LOOKUP(glOrtho,dvec6Func);
  LOOKUP(glClearColor,fvec4Func);
  LOOKUP(glPushMatrix,voidFunc);
  LOOKUP(glPopMatrix,voidFunc);
  LOOKUP(glGetFloatv,uintfloatPFunc);
  LOOKUP(glLineWidth,floatFunc);
  LOOKUP(glFlush,voidFunc);
  LOOKUP(glFinish,voidFunc);
  LOOKUP(glEnable,uintFunc);
  LOOKUP(glDisable,uintFunc);
  LOOKUP(glBlendFunc,uint2Func);
  LOOKUP(glTexCoord2f,fvec2Func);
  LOOKUP(glBindTexture,uint2Func);
  LOOKUP(glGenTextures,uintuintPFunc);
  LOOKUP(glTexParameteri,uint2intFunc);
  LOOKUP(glTexImage2D,glTexImage2DFunc);

  createWindow();
  i[1].devno=1;
  return true;
}

void
SDLGLGUI::createWindow() 
{
  m_flags = SDL_OPENGL|SDL_RESIZABLE;
  if ( m_config.fullscreen) m_flags |= SDL_FULLSCREEN;

  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  resize(m_config.width, m_config.height);
  m_texturePtr=DOPE_SMARTPTR<Texture>(new Texture(*this,"data/textures.png"));
  glDisableP(GL_NORMALIZE);
  glDisableP(GL_LIGHTING);
  glDisableP(GL_CULL_FACE);
}

void
SDLGLGUI::resize(int width, int height)		
{
  // Prevent A Divide By Zero By
  if (width==0)
    width=1;
  if (height==0)			
    height=1;

  if ( SDL_SetVideoMode(width, height, 0, m_flags) == NULL ) {
    throw std::runtime_error(std::string("Couldn't init SDL: ")+SDL_GetError());
  }
  SDL_WM_SetCaption(m_config.title.c_str(), m_config.title.c_str());
  if (m_flags&SDL_FULLSCREEN)
    SDL_ShowCursor(SDL_DISABLE);
  else
    SDL_ShowCursor(SDL_ENABLE);
  m_width=width;
  m_height=height;

  // Reset The Current Viewport
  glViewportP(0,0,width,height);
  // Select The Projection Matrix
  glMatrixModeP(GL_PROJECTION);		
  glLoadIdentityP();
  glOrthoP(0.0f,width,0.0f,height,-100.0f,100.0f);
  glClearColorP(0.0f, 0.0f, 0.0f, 0.0f);
}

void
SDLGLGUI::killWindow()
{
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

bool
SDLGLGUI::step(R dt)
{
  SDL_Event event;
  bool ichanged[2]={false,false};
  while ( SDL_PollEvent(&event) ) {
    switch (event.type) {
    case SDL_QUIT:
      return false;
      break;
    case SDL_VIDEORESIZE:
      resize(event.resize.w,event.resize.h);
      break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      switch (event.key.keysym.sym) {
      case SDLK_LEFT:
	i[0].x=((event.key.type==SDL_KEYDOWN) ? -1 : 0);
	ichanged[0]=true;
	break;
      case SDLK_RIGHT:
	i[0].x=((event.key.type==SDL_KEYDOWN) ? 1 : 0);
	ichanged[0]=true;
	break;
      case SDLK_UP:
	i[0].y=((event.key.type==SDL_KEYDOWN) ? 1 : 0);
	ichanged[0]=true;
	break;
      case SDLK_DOWN:
	i[0].y=((event.key.type==SDL_KEYDOWN) ? -1 : 0);
	ichanged[0]=true;
	break;
      case SDLK_ESCAPE:
      case SDLK_q:
	return false;
	break;
      case SDLK_f:
	if (event.key.type==SDL_KEYDOWN) {
	  if (m_flags&SDL_FULLSCREEN)
	    m_flags&=~SDL_FULLSCREEN;
	  else
	    m_flags|=SDL_FULLSCREEN;
	  resize(m_width,m_height);
	}
	break;
      case SDLK_t:
	m_textureTime=5;
	break;
      default:
	break;
      }
      break;
    default:
      break;
    }
    /*
      case SDL_MOUSEMOTION:
      mouseMotion.emit(event.motion);
      break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
      mouseButton.emit(event.button);
      break;
      case SDL_JOYAXISMOTION:
      joyMotion.emit(event.jaxis);
      break;
      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
      joyButton.emit(event.jbutton);
      break;
      default:
      break;
      }*/
  }
  for (unsigned c=0;c<2;++c)
    if (ichanged[c])
      input.emit(i[c]);

  // Clear The Screen And The Depth Buffer
  //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearP(GL_COLOR_BUFFER_BIT);
  // Reset The View
  glMatrixModeP(GL_MODELVIEW);
  glLoadIdentityP();
  glColor3fP(1.0,1.0,1.0);

  // keep myself in the middle
  const Game::Players &players(m_client.getPlayers());

  const std::vector<uint16_t> &myIDs(m_client.getMyIDs());
  if (!myIDs.empty()) {
    V2D pos;
    unsigned c=0;
    for (unsigned i=0;i<myIDs.size();++i)
      {
	unsigned id=myIDs[i];
	if (id<m_client.getPlayers().size()) {
	  pos+=m_client.getPlayers()[id].m_pos;
	  ++c;
	}
      }
    if (c) {
      pos/=R(c);
      glTranslatefP(-pos[0]+m_width/2,-pos[1]+m_height/2,0);
    }
  }

  // paint world
  Game::WorldPtr worldPtr=m_client.getWorldPtr();
  if (worldPtr.get()) {
    // paint rooms
    for (unsigned r=0;r<worldPtr->getNumRooms();++r)
      {
	// select room color
	bool adic=m_client.getGame().roomIsClosed(r);
	if (adic)
	  glColor3fP(0.5,0.0,0.0);
	else
	  glColor3fP(0.0,0.5,0.0);
	drawPolygon(worldPtr->getLineLoop(r));
	
	/*
	  // paint walls
	  World::EdgeIterator it(*worldPtr.get(),r);
	  Wall wall;
	  for (;it!=World::EdgeIterator(*worldPtr.get());++it) {
	  if (!it.getWall(wall))
	  continue;
	  drawWall(wall);

	  }
	*/
      }
    // paint walls
    unsigned wc=worldPtr->getNumWalls();
    for (unsigned wid=0;wid<wc;++wid)
      {
	Wall w;
	if (worldPtr->getWall(wid,w)) {
	  const FWEdge &e=worldPtr->getEdge(wid);
	  if (m_client.getGame().roomIsClosed(e.m_rcw)||m_client.getGame().roomIsClosed(e.m_rccw))
	    glColor3fP(0.5,0.0,0.0);
	  else
	    glColor3fP(0.0,0.5,0.0);
	  drawWall(w);
	}
      }
    // paint doors
    std::vector<FWEdge::EID> d(worldPtr->getAllDoors());
    Game::Doors &doors(m_client.getGame().getDoors());
    for (unsigned d=0;d<doors.size();++d) {
      if (doors[d].isClosed())
	glColor3fP(0.5,0.0,0.0);
      else
	glColor3fP(0.0,0.0,1.0);
      RealDoor rd(m_client.getGame().doorInWorld(doors[d]));
      Wall w(rd.asWall());
      drawWall(w);
    }
    glColor3fP(1.0,1.0,1.0);    
  }else{
    DOPE_WARN("Did not receive world yet");
  }
  // paint players
  for (unsigned p=0;p<players.size();++p)
    {
      if (m_client.getGame().playerIsLocked(p))
	glColor3fP(1.0,0.0,0.0);
      else
	glColor3fP(0.0,1.0,0.0);
      drawCircle(players[p].m_pos,players[p].m_r);
      V2D dv(V2D(0,100).rot(players[p].getDirection()));
      glColor3fP(1.0,1.0,0.0);
      glBeginP(GL_LINES);
      glVertex2fP(players[p].m_pos[0],players[p].m_pos[1]);
      dv+=players[p].m_pos;
      glVertex2fP(dv[0],dv[1]);
      glEndP();
      glColor3fP(1.0,1.0,1.0);
    }
  // paint texture
  if (m_textureTime>0) {
    glLoadIdentityP();
    glColor4fP(1.0,1.0,1.0,1.0-fabs(m_textureTime-2.5)/2.5);
    m_textureTime-=dt;
    glEnableP(GL_TEXTURE_2D);
    glEnableP(GL_BLEND);
    glBlendFuncP(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTextureP(GL_TEXTURE_2D,m_texturePtr->getTextureID());
    glBeginP(GL_QUADS);
    glTexCoord2fP(0,1);
    glVertex2fP(0,0);
    glTexCoord2fP(1,1);
    glVertex2fP(m_width,0);
    glTexCoord2fP(1,0);
    glVertex2fP(m_width,m_height);
    glTexCoord2fP(0,0);
    glVertex2fP(0,m_height);
    glEndP();
    glDisableP(GL_BLEND);
    glDisableP(GL_TEXTURE_2D);

  }
  glFlushP();
  glFinishP();
  SDL_GL_SwapBuffers();
  return true;
}




void 
SDLGLGUI::drawCircle(const V2D &p, float r)
{
  glPushMatrixP();
  glTranslatefP(p[0], p[1], 0);
  glBeginP(GL_TRIANGLE_FAN);
  glVertex2fP(0, 0);
  unsigned res=12;
  for (unsigned i=0;i<res;++i) {
    double angle(double(i)*2*M_PI/double(res-1));
    glVertex2fP(r * cos(angle), r * sin(angle));
  }
  glEndP();
  glPopMatrixP();
  glFlushP();
}
void
SDLGLGUI::drawWall(const Wall &w)
{
  float lw;
  glGetFloatvP(GL_LINE_WIDTH,&lw);
  glLineWidthP(2*w.getWallWidth());
  Line l(w.getLine());
  drawCircle(l.m_a,w.getPillarRadius());
  drawCircle(l.m_b,w.getPillarRadius());
  glBeginP(GL_LINES);
  glVertex2fP(l.m_a[0],l.m_a[1]);
  glVertex2fP(l.m_b[0],l.m_b[1]);
  glEndP();
  glLineWidthP(lw);
  glFlushP();
}

void
SDLGLGUI::drawPolygon(const std::vector<V2D> &p)
{
  // todo - OpenGL only draws convex polygons => we have to do it ourselves
  // this is a job for Jens Schwarz ;-)
}

