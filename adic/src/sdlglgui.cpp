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
  while ( SDL_PollEvent(&event) ) {
    switch (event.type) {
    case SDL_QUIT:
      return false;
      break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      switch (event.key.keysym.sym) {
      case SDLK_LEFT:
	xChanged.emit((event.key.type==SDL_KEYDOWN) ? -1 : 0);
	break;
      case SDLK_RIGHT:
	xChanged.emit((event.key.type==SDL_KEYDOWN) ? 1 : 0);
	break;
      case SDLK_UP:
	yChanged.emit((event.key.type==SDL_KEYDOWN) ? 1 : 0);
	break;
      case SDLK_DOWN:
	yChanged.emit((event.key.type==SDL_KEYDOWN) ? 1 : 0);
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

  // Clear The Screen And The Depth Buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Reset The View
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(1.0,1.0,1.0);
  // paint world
  Game::WorldPtr worldPtr=m_client.getWorldPtr();
  if (worldPtr.get()) {
    for (unsigned r=0;r<worldPtr->getNumRooms();++r)
      {
	std::vector<V2D> lineloop(worldPtr->getLineLoop(r));
	glBegin(GL_LINE_LOOP);
	for (unsigned p=0;p<lineloop.size();++p)
	  {
	    glVertex2f(lineloop[p][0],lineloop[p][1]);
	  }
	glEnd();
      }
  }else{
    DOPE_WARN("Did not receive world yet");
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
  // Reset The Current Viewport
  glViewport(0,0,width,height);
  // Select The Projection Matrix
  glMatrixMode(GL_PROJECTION);		
  glLoadIdentity();
  // gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
  //  glOrtho(0.0f,width,height,0.0f,-1.0f,1.0f);
  glOrtho(0.0f,width,0.0f,height,-100.0f,100.0f);
  // Select The Modelview Matrix
  glMatrixMode(GL_MODELVIEW);
  // Reset The Modelview Matrix
  glLoadIdentity();
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
  
  resize(width, height);					// Set Up Our Perspective GL Screen
  
  initGL();
}

void
SDLGLGUI::initGL() 
{
  glShadeModel(GL_SMOOTH);
  // Clear The Background Color To Black
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  // Enables Clearing Of The Depth Buffer
  //  glClearDepth(1.0);
  // Enable Depth Testing
  //  glEnable(GL_DEPTH_TEST);
  // Enables Smooth Color Shading
  //  glShadeModel(GL_SMOOTH);
  glShadeModel(GL_FLAT);
  // Enable 2D Texture Mapping
  //  glEnable(GL_TEXTURE_2D);
  //  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  // glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void
SDLGLGUI::killWindow()
{}

