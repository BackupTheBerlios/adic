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

SDLGLGUI::Player::Player(SDLGLGUI &gui, const std::vector<std::string> &uris)
  : time(0)
{
  for (unsigned i=0;i<uris.size();++i)
    textures.push_back(gui.getTexture(uris[i]));
}

void 
SDLGLGUI::Player::step(const ::Player &p,R dt)
{
  if (textures.size()<2) return;
  time+=p.getY()*dt*p.getSpeed().length()*0.1;
  while (time>=textures.size())
	time-=textures.size();
  while (time<0)
    time+=textures.size();
  DOPE_CHECK(time>=0);
  DOPE_CHECK(time<textures.size());
}


SDLGLGUI::SDLGLGUI(Client &client, const GUIConfig &config) 
  : GUI(client,config), m_textureTime(5), 
  m_autoCenter(true), 
  m_zoom(1), m_zoomOp(0), m_autoZoom(true),
  m_showNames(true)
{
  m_scrollOp[0]=m_scrollOp[1]=0;
}


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
  LOOKUP(glScalef,fvec3Func);
  LOOKUP(glBegin,uintFunc);
  LOOKUP(glVertex2i,ivec2Func);
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
  LOOKUP(glRotatef,fvec4Func);

  createWindow();
  i[0].devno=0;
  i[1].devno=1;
  i[2].devno=2;
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
  m_fontPtr=DOPE_SMARTPTR<Texture>(new Texture(*this,"data/font.png"));
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

DOPE_SMARTPTR<Texture> 
SDLGLGUI::getTexture(const std::string &uri)
{
  Textures::iterator it(m_textures.find(uri));
  if (it!=m_textures.end())
    return it->second;
  DOPE_SMARTPTR<Texture> r(new Texture(*this,uri.c_str()));
  m_textures[uri]=r;
  return r;
}

bool
SDLGLGUI::step(R dt)
{
  SDL_Event event;
  bool ichanged[3]={false,false,false};
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
      case SDLK_a:
	i[1].x=((event.key.type==SDL_KEYDOWN) ? -1 : 0);
	ichanged[1]=true;
	break;
      case SDLK_d:
	i[1].x=((event.key.type==SDL_KEYDOWN) ? 1 : 0);
	ichanged[1]=true;
	break;
      case SDLK_w:
	i[1].y=((event.key.type==SDL_KEYDOWN) ? 1 : 0);
	ichanged[1]=true;
	break;
      case SDLK_s:
	i[1].y=((event.key.type==SDL_KEYDOWN) ? -1 : 0);
	ichanged[1]=true;
	break;
	/* third player - this does not work well because of the stupid pc keyboard
	   case SDLK_KP4:
	   i[2].x=((event.key.type==SDL_KEYDOWN) ? -1 : 0);
	   ichanged[2]=true;
	   break;
	   case SDLK_KP6:
	   i[2].x=((event.key.type==SDL_KEYDOWN) ? 1 : 0);
	   ichanged[2]=true;
	   break;
	   case SDLK_KP8:
	   i[2].y=((event.key.type==SDL_KEYDOWN) ? 1 : 0);
	   ichanged[2]=true;
	   break;
	   case SDLK_KP2:
	   i[2].y=((event.key.type==SDL_KEYDOWN) ? -1 : 0);
	   ichanged[2]=true;
	   break;
	*/
      case SDLK_KP4:
	m_scrollOp[0]=((event.key.type==SDL_KEYDOWN) ? -1 : 0);
	m_autoCenter=false;
	break;
      case SDLK_KP6:
	m_scrollOp[0]=((event.key.type==SDL_KEYDOWN) ? 1 : 0);
	m_autoCenter=false;
	break;
      case SDLK_KP8:
	m_scrollOp[1]=((event.key.type==SDL_KEYDOWN) ? 1 : 0);
	m_autoCenter=false;
	break;
      case SDLK_KP2:
	m_scrollOp[1]=((event.key.type==SDL_KEYDOWN) ? -1 : 0);
	m_autoCenter=false;
	break;
      case SDLK_SPACE:
	if (event.key.type==SDL_KEYDOWN)
	  m_showNames=!m_showNames;
	break;
      case SDLK_KP_PLUS:
	if (event.key.type==SDL_KEYDOWN) {
	  m_autoZoom=false;
	  m_zoomOp=1;
	} else
	  m_zoomOp=0;
	break;
      case SDLK_KP_MINUS:
	if (event.key.type==SDL_KEYDOWN) {
	  m_autoZoom=false;
	  m_zoomOp=-1;
	}else
	  m_zoomOp=0;
	break;
      case SDLK_1:
	if (event.key.type==SDL_KEYDOWN) 
	  m_autoZoom=!m_autoZoom;
	break;
      case SDLK_2:
	if (event.key.type==SDL_KEYDOWN) 
	  m_autoCenter=!m_autoCenter;
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
  for (unsigned c=0;c<3;++c)
    if (ichanged[c])
      input.emit(i[c]);

  // Clear The Screen And The Depth Buffer
  //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearP(GL_COLOR_BUFFER_BIT);
  // Reset The View
  glMatrixModeP(GL_MODELVIEW);
  glLoadIdentityP();
  glColor3fP(1.0,1.0,1.0);

  // set up camera
  const Game::Players &players(m_client.getPlayers());

  const std::vector<PlayerID> &myIDs(m_client.getMyIDs());
  if (m_autoCenter||m_autoZoom&&(!myIDs.empty())) {
    V2D pos;
    R big=10000000000.0;
    V2D minp(big,big);
    V2D maxp;
    R maxr=0;
    unsigned c=0;
    for (unsigned i=0;i<myIDs.size();++i)
      {
	unsigned id=myIDs[i];
	// perhaps we did not receive all our players yet
	if (id<m_client.getPlayers().size()) {
	  pos+=m_client.getPlayers()[id].m_pos;
	  minp[0]=std::min(minp[0],m_client.getPlayers()[id].m_pos[0]);
	  minp[1]=std::min(minp[1],m_client.getPlayers()[id].m_pos[1]);
	  maxp[0]=std::max(maxp[0],m_client.getPlayers()[id].m_pos[0]);
	  maxp[1]=std::max(maxp[1],m_client.getPlayers()[id].m_pos[1]);
	  maxr=std::max(maxr,m_client.getPlayers()[id].m_r);
	  ++c;
	}
      }
    if (c) {
      if (m_autoCenter) {
	pos/=R(c);
	// don't use sub-pixels
	pos[0]=int(pos[0]);
	pos[1]=int(pos[1]);
	m_pos=pos;
      }
      if ((c>1)&&m_autoZoom) {
	maxp-=minp;
	maxp+=V2D(maxr,maxr)*2.0+V2D(300,300);
	R xzoom=1.0;
	if (maxp[0]>0)
	  xzoom=m_width/maxp[0];
	R yzoom=1.0;
	if (maxp[1]>0)
	  yzoom=m_height/maxp[1];
	m_zoom=std::min(xzoom,yzoom);
	m_zoom=std::min(R(2.0),m_zoom);
      }
    }
  }
  if (m_scrollOp[0])
    m_pos[0]+=dt*R(m_scrollOp[0]*100)/m_zoom;
  if (m_scrollOp[1])
    m_pos[1]+=dt*R(m_scrollOp[1]*100)/m_zoom;
  if (m_zoomOp) {
    m_zoom*=1.0+0.1*dt*R(m_zoomOp);
    m_zoom=std::max(m_zoom,R(0.1));
  }
  glTranslatefP(-int(m_pos[0]*m_zoom)+m_width/2,-int(m_pos[1]*m_zoom)+m_height/2,0);
  glScalefP(m_zoom,m_zoom,1);

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
  if (m_players.size()!=players.size()) {
    while (players.size()<m_players.size())
      m_players.pop_back();
    while (m_players.size()<players.size())
      {
	PlayerID id=m_players.size();
	if (players[id].isPlayer()) {
	  // find team this player is in
	  unsigned tid=~0U;
	  const std::vector<Team> &teams(m_client.getGame().getTeams());
	  unsigned i=0;
	  for (;i<teams.size();++i) {
	    std::vector<PlayerID>::const_iterator it(std::find(teams[i].playerIDs.begin(),teams[i].playerIDs.end(),id));
	    if (it!=teams[i].playerIDs.end()) {
	      tid=it-teams[i].playerIDs.begin();
	      break;
	    }
	  }
	  DOPE_CHECK(i<teams.size());
	  DOPE_CHECK(tid!=~0U);
	  DOPE_CHECK(tid<teams[i].textures.size());
	  m_players.push_back(Player(*this,teams[i].textures[tid]));
	}else{
	  std::vector<std::string> uris(1);
	  uris[0]=m_client.getPlayerName(id);
	  m_players.push_back(Player(*this,uris));
	}
      }
  }
  for (unsigned p=0;p<players.size();++p)
    {
      if (m_client.getGame().playerIsLocked(p))
	glColor3fP(1.0,0.0,0.0);
      else
	glColor3fP(0.0,1.0,0.0);
      //      drawCircle(players[p].m_pos,players[p].m_r);
      m_players[p].step(players[p],dt);
      drawTexture(m_players[p].getTexture(),players[p].m_pos,players[p].getDirection());
      if (players[p].isPlayer()&&m_showNames) {
	glPushMatrixP();
	glTranslatefP(int(players[p].m_pos[0]), int(players[p].m_pos[1])+2*players[p].m_r, 0);
	drawText(m_client.getPlayerName(p),true);
	glPopMatrixP();
      }
      /*      V2D dv(V2D(0,100).rot(players[p].getDirection()));
      glColor3fP(1.0,1.0,0.0);
      glBeginP(GL_LINES);
      glVertex2fP(players[p].m_pos[0],players[p].m_pos[1]);
      dv+=players[p].m_pos;
      glVertex2fP(dv[0],dv[1]);
      glEndP(); 
      glColor3fP(1.0,1.0,1.0); */
    }
  // paint team statistics
  const std::vector<Team> &teams(m_client.getGame().getTeams());
  if (teams.size()) {
    std::vector<unsigned> numPlayers(teams.size());
    std::vector<unsigned> locked(teams.size());
    for (unsigned i=0;i<teams.size();++i) {
      numPlayers[i]=teams[i].playerIDs.size();
      for (unsigned j=0;j<numPlayers[i];++j) {
	PlayerID id=teams[i].playerIDs[j];
	// perhaps we did not receive this player yet
	if (id<players.size()&&m_client.getGame().playerIsLocked(id))
	  ++locked[i];
      }
    }
    int dx=m_width/(teams.size()+1);
    glPushMatrixP();
    for (unsigned i=0;i<teams.size();++i) {
      glColor3fP(teams[i].color[0],teams[i].color[1],teams[i].color[2]);
      glLoadIdentityP();
      glTranslatefP(dx*(i+1),40,0);
      drawText(teams[i].name,true);
      glLoadIdentityP();
      glTranslatefP(dx*(i+1),10,0);
      drawText(anyToString(numPlayers[i]-locked[i])+"/"+anyToString(numPlayers[i]),true);
      glFlushP();
    }
    glPopMatrixP();
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

V2D 
SDLGLGUI::getPos() const
{
  return m_pos;
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
  glLineWidthP(2*w.getWallWidth()*m_zoom);
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
void
SDLGLGUI::drawTexture(const Texture &tex, const V2D &p, R rot)
{
  int w=tex.getWidth();
  int h=tex.getHeight();
  int w2=w>>1;
  int h2=h>>1;
  glPushMatrixP();
  glTranslatefP(p[0], p[1], 0);
  rot=-rot*180/M_PI;
  glRotatefP(rot+90,0,0,1);
  glColor3fP(1.0,1.0,1.0);
  glEnableP(GL_TEXTURE_2D);
  glEnableP(GL_BLEND);
  glBlendFuncP(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindTextureP(GL_TEXTURE_2D,tex.getTextureID());
  glBeginP(GL_QUADS);
  glTexCoord2fP(0,1);
  glVertex2fP(-w2,-h2);
  glTexCoord2fP(1,1);
  glVertex2fP(w2,-h2);
  glTexCoord2fP(1,0);
  glVertex2fP(w2,h2);
  glTexCoord2fP(0,0);
  glVertex2fP(-w2,h2);
  glEndP();
  glDisableP(GL_BLEND);
  glDisableP(GL_TEXTURE_2D);
  glPopMatrixP();
}

void 
SDLGLGUI::drawText(const std::string &text, bool centered)
{
  int xtiles=16,ytiles=16;
  int w=m_fontPtr->getWidth(),h=m_fontPtr->getHeight();
  float fw=w,fh=h;
  int dx=w/xtiles;
  int dy=h/ytiles;
  float fdx=1.0f/xtiles;
  float fdy=1.0f/ytiles;
  int offset=0;
  if (centered) offset=-int(text.size())*dx/2;
  glEnableP(GL_TEXTURE_2D);
  glEnableP(GL_BLEND);
  glBlendFuncP(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindTextureP(GL_TEXTURE_2D, m_fontPtr->getTextureID());
  glBeginP(GL_QUADS);
  for (unsigned i=0;i<text.size();++i){
    float cx=float((int(text[i]-32)%xtiles)*dx)/fw;
    float cy=float((int(text[i]-32)/xtiles)*dy)/fh;
    int x=i*dx+offset;
    glTexCoord2fP(cx,cy);
    glVertex2iP(x,dy);
    glTexCoord2fP(cx+fdx,cy);
    glVertex2iP(x+dx,dy);
    glTexCoord2fP(cx+fdx,cy+fdy);
    glVertex2iP(x+dx,0);
    glTexCoord2fP(cx,cy+fdy);
    glVertex2iP(x,0);
  }
  glEndP();
  glDisableP(GL_BLEND);
  glDisableP(GL_TEXTURE_2D);
}
