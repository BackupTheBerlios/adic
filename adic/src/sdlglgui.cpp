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

bool
SDLGLGUI::init()
{
  initSDL();
  if (SDL_GL_LoadLibrary(m_config.libGL.c_str())==-1)
    throw std::runtime_error(std::string("Could not load OpenGL lib: \"")+m_config.libGL.c_str()+"\": "+SDL_GetError());
  LOOKUP(glClear,uintFunc);
  LOOKUP(glMatrixMode,uintFunc);
  LOOKUP(glLoadIdentity,voidFunc);
  LOOKUP(glColor3f,fvec3Func);
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

  createWindow(m_config.title.c_str(),m_config.width,m_config.height,m_config.bits,m_config.fullscreen);
  return true;
}

bool
SDLGLGUI::step(R dt)
{
  SDL_Event event;
  bool ichanged=false;
  while ( SDL_PollEvent(&event) ) {
    switch (event.type) {
    case SDL_QUIT:
      return false;
      break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      switch (event.key.keysym.sym) {
      case SDLK_LEFT:
	i.x=((event.key.type==SDL_KEYDOWN) ? -1 : 0);
	ichanged=true;
	break;
      case SDLK_RIGHT:
	i.x=((event.key.type==SDL_KEYDOWN) ? 1 : 0);
	ichanged=true;
	break;
      case SDLK_UP:
	i.y=((event.key.type==SDL_KEYDOWN) ? 1 : 0);
	ichanged=true;
	break;
      case SDLK_DOWN:
	i.y=((event.key.type==SDL_KEYDOWN) ? -1 : 0);
	ichanged=true;
	break;
      case SDLK_ESCAPE:
      case SDLK_q:
	return false;
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
  if (ichanged)
    input.emit(i);

  // Clear The Screen And The Depth Buffer
  //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearP(GL_COLOR_BUFFER_BIT);
  // Reset The View
  glMatrixModeP(GL_MODELVIEW);
  glLoadIdentityP();
  glColor3fP(1.0,1.0,1.0);

  // keep myself in the middle
  const Game::Players &players(m_client.getPlayers());

  uint16_t me=m_client.getPlayerID();
  if (me<players.size()) {
    V2D mypos(m_client.getPlayers()[me].m_pos);
    glTranslatefP(-mypos[0]+m_width/2,-mypos[1]+m_height/2,0);
  }

  // paint world
  Game::WorldPtr worldPtr=m_client.getWorldPtr();
  if (worldPtr.get()) {
    // paint rooms
    for (unsigned r=0;r<worldPtr->getNumRooms();++r)
      {
	// select room color
	RealRoom room(*worldPtr.get(),m_client.getGame().getDoors(),r);
	bool adic=room.getADIC();
	if (adic)
	  glColor3fP(0.5,0.0,0.0);
	else
	  glColor3fP(0.0,0.5,0.0);
	drawPolygon(worldPtr->getLineLoop(r));

	// paint walls
	World::EdgeIterator it(*worldPtr.get(),r);
	Wall wall;
	for (;it!=World::EdgeIterator(*worldPtr.get());++it) {
	  if (!it.getWall(wall))
	    continue;
	  drawWall(wall);
	}
      }
    // paint doors
    std::vector<FWEdge::EID> d(worldPtr->getAllDoors());
    Game::Doors &doors(m_client.getGame().getDoors());
    glColor3fP(0.0,0.0,1.0);
    for (unsigned d=0;d<doors.size();++d) {
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
  glFlushP();
  //glFinishP();
  SDL_GL_SwapBuffers();
  return true;
}

//! Resize And Initialize The GL Window
void SDLGLGUI::resize(int width, int height)		
{
  // Prevent A Divide By Zero By
  if (height==0)			
    height=1;

  m_width=width;
  m_height=height;

  // Reset The Current Viewport
  glViewportP(0,0,width,height);
  // Select The Projection Matrix
  glMatrixModeP(GL_PROJECTION);		
  glLoadIdentityP();
  glOrthoP(0.0f,width,0.0f,height,-100.0f,100.0f);
}

void SDLGLGUI::initSDL() {
  /* Initialize SDL */
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    throw std::runtime_error(std::string("Couldn't init SDL: ")+SDL_GetError());
}

void
SDLGLGUI::createWindow(const char* title, int width, int height, int bits, bool fullscreenflag) 
{
  Uint32 flags;
  
  flags = SDL_OPENGL;
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  if ( fullscreenflag ) {
    flags |= SDL_FULLSCREEN;
  }
  if ( SDL_SetVideoMode(width, height, 0, flags) == NULL ) {
    throw std::runtime_error(std::string("Couldn't init SDL: ")+SDL_GetError());
  }
  SDL_WM_SetCaption(title, "opengl");
  if (fullscreenflag)
    SDL_ShowCursor(SDL_DISABLE);
  resize(width, height);					// Set Up Our Perspective GL Screen
  
  initGL();
}

void
SDLGLGUI::initGL() 
{
  glClearColorP(0.0f, 0.0f, 0.0f, 0.0f);
  /*
  glShadeModel(GL_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  */
}

void
SDLGLGUI::killWindow()
{
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void 
SDLGLGUI::drawCircle(const V2D &p, float r)
{
  double angle;

  glPushMatrixP();
  glTranslatefP(p[0], p[1], 0);
  glBeginP(GL_TRIANGLE_FAN);
  glVertex2fP(0, 0);
  for (angle = 0.0; angle <= 2 * M_PI; angle += M_PI / 12) {
    glVertex2fP(r * cos(angle), r * sin(angle));
  }
  glEndP();
  glPopMatrixP();
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
}

void
SDLGLGUI::drawPolygon(const std::vector<V2D> &p)
{
  // todo - OpenGL only draws convex polygons => we have to do it ourselves
  // this is a job for Jens Schwarz ;-)
}

