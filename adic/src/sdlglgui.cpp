#include "sdlglgui.h"

#include <SDL/SDL.h>
#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>	// Header File For The OpenGL32 Library
#include <OpenGL/glu.h>	// Header File For The GLu32 Library
#else
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
#endif

bool
SDLGLGUI::init()
{
  initSDL();
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
  glClear(GL_COLOR_BUFFER_BIT);
  // Reset The View
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(1.0,1.0,1.0);

  // keep myself in the middle
  const Game::Players &players(m_client.getPlayers());

  uint16_t me=m_client.getPlayerID();
  if (me<players.size()) {
    V2D mypos(m_client.getPlayers()[me].m_pos);
    glTranslatef(-mypos[0]+m_width/2,-mypos[1]+m_height/2,0);
  }

  // paint world
  Game::WorldPtr worldPtr=m_client.getWorldPtr();
  if (worldPtr.get()) {
    // paint walls
    for (unsigned r=0;r<worldPtr->getNumRooms();++r)
      {
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
    glColor3f(0.0,0.0,1.0);
    for (unsigned d=0;d<doors.size();++d) {
      RealDoor rd(m_client.getGame().doorInWorld(doors[d]));
      Wall w(rd.asWall());
      drawWall(w);
    }
    glColor3f(1.0,1.0,1.0);    
  }else{
    DOPE_WARN("Did not receive world yet");
  }
  // paint players
  for (unsigned p=0;p<players.size();++p)
    {
      glColor3f(0.0,1.0,0.0);
      drawCircle(players[p].m_pos,players[p].m_r);
      V2D dv(V2D(0,100).rot(players[p].getDirection()));
      glColor3f(1.0,1.0,0.0);
      glBegin(GL_LINES);
      glVertex2f(players[p].m_pos[0],players[p].m_pos[1]);
      dv+=players[p].m_pos;
      glVertex2f(dv[0],dv[1]);
      glEnd();
      glColor3f(1.0,1.0,1.0);
    }
  
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
  glViewport(0,0,width,height);
  // Select The Projection Matrix
  glMatrixMode(GL_PROJECTION);		
  glLoadIdentity();
  glOrtho(0.0f,width,0.0f,height,-100.0f,100.0f);
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
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

  glPushMatrix();
  glTranslatef(p[0], p[1], 0);
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(0, 0);
  for (angle = 0.0; angle <= 2 * M_PI; angle += M_PI / 12) {
    glVertex2f(r * cos(angle), r * sin(angle));
  }
  glEnd();
  glPopMatrix();
}
void
SDLGLGUI::drawWall(const Wall &w)
{
  float lw;
  glGetFloatv(GL_LINE_WIDTH,&lw);
  glLineWidth(2*w.getWallWidth());
  Line l(w.getLine());
  drawCircle(l.m_a,w.getPillarRadius());
  drawCircle(l.m_b,w.getPillarRadius());
  glBegin(GL_LINES);
  glVertex2f(l.m_a[0],l.m_a[1]);
  glVertex2f(l.m_b[0],l.m_b[1]);
  glEnd();
  glLineWidth(lw);
}
