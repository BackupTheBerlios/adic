#include "sdlglgui.h"
#include "sdlmenu.h"
#include <algorithm>
#include <iomanip>


SDLGLGUI::SDLGLGUI(Client &client) 
  : GUI(client), m_terminal(*this,0,getGUIConfig().height-48),
    //    m_textureTime(5), 
    m_autoCenter(true), 
    m_zoomOp(0), m_autoZoom(true),
    m_showNames(true),
    m_quit(false), m_frames(0),
    m_chatMode(0), m_lineSmooth(getGUIConfig().quality>1), m_toggles(~0)
{
  m_scrollOp[0]=m_scrollOp[1]=0;
  sf.quitSignal.connect(SigC::slot(*this,&SDLGLGUI::handleQuit));
  sf.resize.connect(SigC::slot(*this,&SDLGLGUI::handleResize));
  m_terminal.printed.connect(printed.slot());
  m_start.now();
}

SDLGLGUI::~SDLGLGUI()
{
  // no gl/glu commands are allowed after killWindow
  // because if we use dlopen the libraries are unloaded
  // => we have to deinit all GL related stuff here
  m_textures.clear();
  m_polys.clear();
  killWindow();
}

bool
SDLGLGUI::init()
{
  // Initialize SDL

  // first init
  if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) < 0 )
    throw std::runtime_error(std::string("Couldn't init SDL: ")+SDL_GetError());
  DEBUG_GL("sdl init success");
  
#ifdef DLOPEN_OPENGL
  loadGL(getGUIConfig().libGL);
  loadGLU(getGUIConfig().libGLU);
  lookupGLSymbols();
#endif  

  int major=2;
  int minor=0;
  int8_t devno=0;
  while (major>=0) {
    InputDevName devname;
    devname.major=major;
    devname.minor=minor;
    devname.devno=devno;

    DOPE_SMARTPTR<SDLInputDev> dev(SDLInputDev::create(sf,devname));
    if (dev.get()) {
      dev->input.connect(input.slot());
      m_inputDevices.push_back(dev);
      ++minor;
      ++devno;
    }else{
      --major;
      minor=0;
    }
  }
  // important to connect this after the input devices have been connected
  sf.keyEvent.connect(SigC::slot(*this,&SDLGLGUI::handleKey));

  // chat input field
  // 1. connect key source to input field
  sf.keyEvent.connect(SigC::slot(m_chatLine,&SDLInputField::handleKey));
  // forward printed signal (sound)
  m_chatLine.printed.connect(printed.slot());
  // handle input
  m_chatLine.input.connect(SigC::slot(*this,&SDLGLGUI::handleChatInput));

  DEBUG_GL("prepare to call createwindow");
  std::cerr << std::flush;
  TimeStamp t(2,0);
  t.sleep();
  DEBUG_GL("calling createwindow");
  createWindow();

  // create menu
  m_menuPtr=DOPE_SMARTPTR<SDLMenu>(new SDLMenu(*this));
  assert(m_inputDevices.size());
  assert(m_menuPtr.get());
  for (unsigned i=0;i<m_inputDevices.size();++i)
    // we do not want the mouse as input device for the menu
    if (!m_inputDevices[i]->getDevName().isMouse())
      m_inputDevices[i]->input.connect(SigC::slot(*m_menuPtr,&SDLMenu::handleInput));
  m_menuPtr->serverSelected.connect(SigC::slot(m_client,&Client::connect));
  m_menuPtr->configured.connect(SigC::slot(m_client,&Client::sendGreeting));
  m_menuPtr->printed.connect(printed.slot());
  DEBUG_GL("init complete");
  GL_ERRORS();
  return true;
}

void
SDLGLGUI::createWindow() 
{
  m_flags = SDL_OPENGL|SDL_RESIZABLE;
  if ( getGUIConfig().fullscreen) m_flags |= SDL_FULLSCREEN;

  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  SDL_ResizeEvent e;
  e.w=getGUIConfig().width;
  e.h=getGUIConfig().height;

  DEBUG_GL("creating window by resize event");

  sf.resize.emit(e);
  //  resize(getGUIConfig().width, getGUIConfig().height);
  //  m_texturePtr=getTexture("data:gui_0001.png");
  m_fontTexPtr=getTexture("data:font.png");
  m_fontPtr=DOPE_SMARTPTR<GLFont>(new GLFont(m_fontTexPtr));
  m_circlePtr=getTexture("data:pillar.png");
  m_texCircle=true;

  // load the room textures - in the moment they are hardcoded
  std::vector<std::string> texNames;
  texNames.push_back("data:room1.png");
  texNames.push_back("data:room2.png");
  texNames.push_back("data:room3.png");
  texNames.push_back("data:room4.png");
  m_roomTextures.resize(texNames.size());
  for (unsigned i=0;i<texNames.size();++i)
    m_roomTextures[i]=getTexture(texNames[i].c_str());

  glDisable(GL_NORMALIZE);
  GL_ERRORS();
  glDisable(GL_LIGHTING);
  GL_ERRORS();
  glDisable(GL_CULL_FACE);
  GL_ERRORS();
  float range[2];
  glGetFloatv(GL_LINE_WIDTH_RANGE,range);
  GL_ERRORS();
  //  std::cerr << range[0] << "<LINE_WIDTH<"<<range[1]<<"\n";
  if (!m_lineSmooth) {
    glDisable(GL_LINE_SMOOTH);
    GL_ERRORS();
  }else{
    glEnable(GL_LINE_SMOOTH);
    GL_ERRORS();
  }
  glShadeModel(GL_FLAT);
  GL_ERRORS();
  if (getGUIConfig().quality<=1) {
    glDisable(GL_DITHER);
    GL_ERRORS();
  }
}

void
SDLGLGUI::resize(int width, int height)		
{
  // Prevent A Divide By Zero By
  if (width==0)
    width=1;
  if (height==0)			
    height=1;

  // todo: resize should not set video mode
  if ( SDL_SetVideoMode(width, height, 0, m_flags) == NULL ) {
    throw std::runtime_error(std::string("Couldn't init SDL: ")+SDL_GetError());
  }
  SDL_WM_SetCaption(getGUIConfig().title.c_str(), getGUIConfig().title.c_str());
  if (m_flags&SDL_FULLSCREEN)
    SDL_ShowCursor(SDL_DISABLE);
  else
    SDL_ShowCursor(SDL_ENABLE);
  m_width=width;
  m_height=height;

  int db=0;
  if (SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &db )) DOPE_WARN("could not get attribute");
  if (!db) DOPE_WARN("did not get double buffer");

  // Reset The Current Viewport
  glViewport(0,0,width,height);
  GL_ERRORS();
  // Select The Projection Matrix
  glMatrixMode(GL_PROJECTION);		
  GL_ERRORS();
  glLoadIdentity();
  GL_ERRORS();
  glOrtho(0.0f,width,0.0f,height,-100.0f,100.0f);
  GL_ERRORS();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  GL_ERRORS();
}

void
SDLGLGUI::killWindow()
{
  // set all function pointers zero
#ifdef DLOPEN_OPENGL
  deinitGLSymbols();
#endif
  SDL_QuitSubSystem(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK);
}

DOPE_SMARTPTR<Texture> 
SDLGLGUI::getTexture(const std::string &uri)
{
  Textures::iterator it(m_textures.find(uri));
  if (it!=m_textures.end())
    return it->second;
  // todo this is nearly the same as in the uriloader class
  // there should be one central location for the vfs stuff
  // the problem is with c-libraries either expecting a file name
  // or at least a fd
  if (uri.length()>5) {
    std::string scheme(uri,0,5); 
    if (scheme=="data:") {
      std::string dataFile(uri,5);
      std::string fname(findDataFile(dataFile));
      if (fname.empty())
	throw ResourceNotFound(uri,std::string("Data file \"")+dataFile+"\" does not exist in path");
      DOPE_SMARTPTR<Texture> r(new Texture(fname.c_str(),getGUIConfig().quality));
      m_textures[uri]=r;
      return r;
    }
    throw ResourceNotFound(uri,std::string("Unsupported scheme: \"")+scheme+"\"");
  }
  throw ResourceNotFound(uri,std::string("Unsupported URI: \"")+uri+"\"");
}

bool
SDLGLGUI::handleKey(SDL_KeyboardEvent e)
{
  bool pressed=e.state==SDL_PRESSED;
  SDLKey k(e.keysym.sym);
  if (m_chatLine.isActive())
    return false;
  switch (k) {
  case SDLK_LEFT:
    if (m_menuPtr.get()) {
      if (!pressed) return true;
      // synthesize right pressed event for menu
      Input i;
      i.x=-1;
      m_menuPtr->handleInput(i);
      return true;
    }
    m_scrollOp[0]=(pressed ? -1 : 0);
    if (pressed) m_autoCenter=false;
    return true;
  case SDLK_RIGHT:
    if (m_menuPtr.get()) {
      if (!pressed) return true;
      // synthesize right pressed event for menu
      Input i;
      i.x=1;
      m_menuPtr->handleInput(i);
      return true;
    }
    m_scrollOp[0]=(pressed ? 1 : 0);
    if (pressed) m_autoCenter=false;
    return true;
  case SDLK_UP:
    if (m_menuPtr.get()) {
      if (!pressed) return true;
      // synthesize right pressed event for menu
      Input i;
      i.y=1;
      m_menuPtr->handleInput(i);
      return true;
    }
    m_scrollOp[1]=(pressed ? 1 : 0);
    if (pressed) m_autoCenter=false;
    return true;
  case SDLK_DOWN:
    if (m_menuPtr.get()) {
      if (!pressed) return true;
      // synthesize right pressed event for menu
      Input i;
      i.y=-1;
      m_menuPtr->handleInput(i);
      return true;
    }
    m_scrollOp[1]=(pressed ? -1 : 0);
    if (pressed) m_autoCenter=false;
    return true;
  case SDLK_SPACE:
    if (pressed)
      m_showNames=!m_showNames;
    return true;
  case SDLK_KP_PLUS:
    if (pressed) {
      m_autoZoom=false;
      m_zoomOp=1;
    } else
      m_zoomOp=0;
    return true;
  case SDLK_KP_MINUS:
    if (pressed) {
      m_autoZoom=false;
      m_zoomOp=-1;
    }else
      m_zoomOp=0;
    return true;
  case SDLK_1:
    if (pressed) 
      m_autoZoom=!m_autoZoom;
    return true;
  case SDLK_2:
    if (pressed) 
      m_autoCenter=!m_autoCenter;
    return true;
  case SDLK_ESCAPE:
    m_quit=true;
    return true;
  case SDLK_f:
    if (pressed) {
      if (m_flags&SDL_FULLSCREEN)
	m_flags&=~SDL_FULLSCREEN;
      else
	m_flags|=SDL_FULLSCREEN;
      resize(m_width,m_height);
    }
    return true;
  case SDLK_t:
    m_chatLine.setActive(true);
    m_chatMode=1;
    return true;
  case SDLK_g:
    m_chatLine.setActive(true);
    m_chatMode=2;
    return true;
  case SDLK_RETURN:
    if (m_menuPtr.get()) {
      if (!pressed) return true;
      // synthesize right pressed event for menu
      Input i;
      i.x=1;
      m_menuPtr->handleInput(i);
      return true;
    }
    break;
  case SDLK_c:
    if (pressed) m_texCircle=!m_texCircle;
    return true;
    break;
  case SDLK_l:
    if (pressed) {
      m_lineSmooth=!m_lineSmooth;
      if (!m_lineSmooth) glDisable(GL_LINE_SMOOTH);
      else glEnable(GL_LINE_SMOOTH);
    }
    return true;
    break;
  case SDLK_3:
  case SDLK_4:
  case SDLK_5:
  case SDLK_6:
  case SDLK_7:
  case SDLK_8:
  case SDLK_9:
    {
      if (!pressed)
	return true;
      Uint8 bit=((Uint8)k-(Uint8)SDLK_0);
      unsigned mask=1<<bit;
      bool on=(m_toggles&mask);
      if (on) m_toggles&=~mask;
      else m_toggles|=mask;
      return true;
    }
    break;
  case SDLK_p:
    if (pressed) m_client.ping();
    break;
  case SDLK_m:
    //    m_textureTime=5;
    return true;
    break;
  default:
    return false;
  }
  return false;
}

void
SDLGLGUI::handleResize(SDL_ResizeEvent e)
{
  resize(e.w,e.h);
}

void
SDLGLGUI::handleQuit()
{
  m_quit=true;
}

void
SDLGLGUI::handleChatInput(const std::string &msg)
{
  ChatMessage m;
  m.message=msg;
  m.global=(m_chatMode==1);
  chatMessage.emit(m);
  m_chatMode=0;
  m_chatLine.clear();
}

void
SDLGLGUI::handleNewClient(DOPE_SMARTPTR<NewClient> mPtr)
{
  if (!m_menuPtr.get())
    return;
  m_menuPtr->handleNewClient(mPtr);
}

void
SDLGLGUI::handleEndGame(DOPE_SMARTPTR<EndGame> egPtr)
{
  // prepare for next level - reset all internal data structures
  m_animations.clear();
  m_polys.clear();
}

bool
SDLGLGUI::step(R dt)
{
  static SDL_Surface* test=NULL;
  
  if (m_quit) return false;
  sf.produce();

  // Clear The Screen And The Depth Buffer
  //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT);
  // Reset The View
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(1.0,1.0,1.0);

  // set up camera
  setupCamera(dt);
  
  // paint world (if possible)
  const Game::WorldPtr &worldPtr(m_client.getWorldPtr());
  if (worldPtr.get()) {
    if (!m_haveWorld) {
      // got new world => set up camera
      const V2D& tl(worldPtr->getTopLeft());
      const V2D& br(worldPtr->getBottomRight());
      V2D p(tl+br);
      p*=0.5;
      R xzoom=double(m_width)/(br[0]-tl[0]);
      R yzoom=double(m_height)/(tl[1]-br[1]);
      m_camera=Camera(p,std::min(std::min(xzoom,yzoom)*R(0.9),R(2.0)),10);
      m_camera.setZoom(1);
    }
    m_haveWorld=true;
    
    if (m_toggles&(1<<8)) drawPolys();
    if (m_toggles&(1<<3)) drawWalls();
    if (m_toggles&(1<<4)) drawDoors();
    if (m_toggles&(1<<5)) drawPillars();
    if (m_toggles&(1<<6)) drawPlayers(dt);
    if (m_toggles&(1<<7)) drawTeamStat();
  }else
    m_haveWorld=false;
  
  
  // paint texture
  /* left here for testing purposes
  if (m_textureTime>0) {
    m_textureTime-=dt;

    if (m_toggles&(1<<9)) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      float alpha=1.0-fabs(m_textureTime-2.5)/2.5;
      if (getGUIConfig().quality<=0) alpha=(alpha > 0.1) ? 1.0 : 0.0;
      glColor4f(1.0,1.0,1.0,alpha);
    }
    if (m_toggles&(1<<8)) {
      if (!test) {
	test=IMG_Load("data:gui_0001.png");
	if (!test) DOPE_FATAL("Could not load image");
	SDL_LockSurface(test);
	char *p=(char *)(test->pixels);
	// todo - we should do more checks
	int bprow=test->w*test->format->BytesPerPixel;
	DOPE_CHECK(bprow==int(test->pitch));
	char *row=new char[bprow];
	for (int r=0;r<test->h/2;++r) {
	  char *src=p+r*bprow;
	  char *dest=p+((test->h)-1-r)*bprow;
	  memcpy((void *)row,(void *)src,bprow);
	  memcpy((void *)src,(void *)dest, bprow);
	  memcpy((void *)dest,(void *)row,bprow);
	}
	delete [] row;
	SDL_UnlockSurface(test);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glPixelZoom(1.0,1.0);
      }
      glLoadIdentity();
      glRasterPos2i(0,0);
      //      SDL_LockSurface(test);
      glDrawPixels(test->w, test->h, GL_RGBA, GL_UNSIGNED_BYTE, test->pixels);
      //      SDL_UnlockSurface(test);
    }else{
      glLoadIdentity();
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,m_texturePtr->getTextureID());
      float mx=m_width/2-0.375;
      float my=m_height/2-0.375;
      float dx=m_texturePtr->getWidth()/2;
      float dy=m_texturePtr->getHeight()/2;
      glBegin(GL_QUADS);
      glTexCoord2f(0,1);
      glVertex2f(mx-dx,my-dy);
      glTexCoord2f(1,1);
      glVertex2f(mx+dx,my-dy);
      glTexCoord2f(1,0);
      glVertex2f(mx+dx,my+dy);
      glTexCoord2f(0,0);
      glVertex2f(mx-dx,my+dy);
      glEnd();
      glDisable(GL_TEXTURE_2D);
    }
    glDisable(GL_BLEND);
  }
  */

  // paint FPS
  TimeStamp ct;
  ct.now();
  ct-=m_start;
  R uptime=R(ct.getSec())+(R(ct.getUSec())/1000000);
  ++m_frames;
  glColor3f(1.0,1.0,1.0);
  if (!(m_toggles&(1<<9))) {
    std::ostringstream o;
    o << "Up: " << std::fixed << std::setprecision(1) << std::setw(7) << uptime 
      << "\nFPS: " << std::setw(6) << R(m_frames)/uptime;
    //	      << " Frame: " << std::setw(10) << m_frames;
    glLoadIdentity();
    glTranslatef(0,m_height-m_fontPtr->getHeight(),0);
    m_fontPtr->drawText(o.str());
  }

  // paint terminal
  m_terminal.step(dt);
  // paint chat line
  glLoadIdentity();
  glTranslatef(0,0,0);
  glColor3f(1.0,1.0,1.0);
  m_fontPtr->drawText(m_chatLine.getContent());

  // paint menu
  if (m_menuPtr.get())
    if (!m_menuPtr->step(dt))
      m_menuPtr=DOPE_SMARTPTR<SDLMenu>();

  flush();
  glFinish();
  SDL_GL_SwapBuffers();
  return true;
}

void 
SDLGLGUI::drawCircle(const V2D &p, float r)
{
  if (!m_texCircle) {
    unsigned d=1.0/getZoom();
    unsigned res=12;
    if (d>1) res/=d;
    if (res>=3) {
      glPushMatrix();
      glTranslatef(p[0], p[1], 0);
      glBegin(GL_TRIANGLE_FAN);
      glVertex2f(0, 0);
      for (unsigned i=0;i<res;++i) {
	double angle(double(i)*2*M_PI/double(res-1));
	glVertex2f(r * cos(angle), r * sin(angle));
      }
      glEnd();
      glPopMatrix();
      flush();
    }
  }
  else
    {
      // hand crafted drawTexture
      // drawTexture(*m_circlePtr,p);
      const Texture &tex(*m_circlePtr.get());
      int w=tex.getWidth();
      int h=tex.getHeight();
      int w2=w>>1;
      int h2=h>>1;

      glPushMatrix();
      glTranslatef(p[0], p[1], 0);

      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBindTexture(GL_TEXTURE_2D,tex.getTextureID());

      glBegin(GL_QUADS);
      glTexCoord2f(0,1);
      glVertex2f(-w2,-h2);
      glTexCoord2f(1,1);
      glVertex2f(w2,-h2);
      glTexCoord2f(1,0);
      glVertex2f(w2,h2);
      glTexCoord2f(0,0);
      glVertex2f(-w2,h2);
      glEnd();

      glDisable(GL_BLEND);
      glDisable(GL_TEXTURE_2D);
      glPopMatrix();
    }
}
void
SDLGLGUI::drawWall(const Wall &w, bool cwClosed, bool ccwClosed)
{
  // backup line width
  float lw;
  glGetFloatv(GL_LINE_WIDTH,&lw);

  int q=getGUIConfig().quality;
  float nlw;
  int numlines;
  if (q<=1)
    // in low quality mode we only draw 1 or 2 lines
    numlines=(cwClosed==ccwClosed) ? 1 : 2;
  else numlines=3;
  float wt=w.getWallWidth();
  nlw=wt*getZoom()/float(numlines);
  if (m_lineSmooth) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
  }
  // now set the line width
  glLineWidth(nlw);

  Line l(w.getLine());
  const V2D &ln(l.normal());
  if (numlines>1) {
    // draw clockwise side
    V2D a(l.m_a);
    V2D b(l.m_b);
    float o=wt/numlines;
    a+=ln*o;
    b+=ln*o;
    if (cwClosed) glColor3f(0.5,0.0,0.0);
    else glColor3f(0.0,0.5,0.0);
    glBegin(GL_LINES);
    glVertex2f(a[0],a[1]);
    glVertex2f(b[0],b[1]);
    glEnd();
    // draw counter clockwise side
    a=l.m_a;
    b=l.m_b;
    a+=ln*-o;
    b+=ln*-o;
    if (ccwClosed) glColor3f(0.5,0.0,0.0);
    else glColor3f(0.0,0.5,0.0);
    glBegin(GL_LINES);
    glVertex2f(a[0],a[1]);
    glVertex2f(b[0],b[1]);
    glEnd();
  }
  if (numlines!=2) {
    // draw middle    
    float hs=(numlines<3) ? 0.5f : 0.7f;
    if (cwClosed&&ccwClosed) glColor3f(hs,0.0,0.0);
    else if (cwClosed||ccwClosed) glColor3f(hs,0.4,0.0);
    else glColor3f(0.0,hs,0.0);
    V2D a(l.m_a);
    V2D b(l.m_b);
    glBegin(GL_LINES);
    glVertex2f(a[0],a[1]);
    glVertex2f(b[0],b[1]);
    glEnd();
  }

  // restore linewidth
  glLineWidth(lw);
  if (m_lineSmooth) glDisable(GL_BLEND);
}

void
SDLGLGUI::drawDoor(const Wall &w, bool closed)
{
  // backup line width
  float lw;
  glGetFloatv(GL_LINE_WIDTH,&lw);

  // now set the line width
  glLineWidth(w.getWallWidth()*getZoom());

  Line l(w.getLine());

  // draw 
  const V2D &a(l.m_a);
  const V2D &b(l.m_b);
  if (closed) glColor3f(0.5,0.0,0.0);
  else glColor3f(0.0,0.0,0.5);
  glBegin(GL_LINES);
  glVertex2f(a[0],a[1]);
  glVertex2f(b[0],b[1]);
  glEnd();

  // restore linewidth
  glLineWidth(lw);

  // draw pillar
  glColor3f(1.0,1.0,1.0);
  drawCircle(l.m_b,w.getPillarRadius());
  //  drawCircle(l.m_b,w.getPillarRadius());
}

void
SDLGLGUI::drawPolys()
{
  if (getGUIConfig().quality<1) return;
  const Game::WorldPtr &worldPtr(m_client.getWorldPtr());
  unsigned nr=worldPtr->getNumRooms();
  if (nr!=m_polys.size()) {
    m_polys.resize(nr);
    DOPE_MSG("Info:","start tesselating polygons\n"<<std::flush);
    for (unsigned i=0;i<nr;++i){
      m_polys[i]=DOPE_SMARTPTR<GLPoly>(new GLPoly(worldPtr->getRoomPoly(i).getLineLoop()));
    }
    DOPE_MSG("Info:","finished tesselating polygons\n"<<std::flush);
  }
  float cschemes[4][3]={
    {1.0,0.7,0.7},
    {0.8,1.0,0.8},
    {0.2,0.0,0.0},
    {0.0,0.2,0.0}
  };

  float* colors[2]={cschemes[2],cschemes[3]};
  if (getGUIConfig().quality>1) {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    colors[0]=cschemes[0];
    colors[1]=cschemes[1];
  }
  for (unsigned i=0;i<nr;++i) {
    int c=m_client.getGame().roomIsClosed(i) ? 0 : 1;
    glColor3fv(colors[c]);
    glBindTexture(GL_TEXTURE_2D,m_roomTextures[i%m_roomTextures.size()]->getTextureID());
    m_polys[i]->draw();
  }
  if (getGUIConfig().quality>1) {
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
  }
}

void
SDLGLGUI::drawTexture(const Texture &tex, const V2D &p, R rot)
{
  int w=tex.getWidth();
  int h=tex.getHeight();
  int w2=w>>1;
  int h2=h>>1;
  glPushMatrix();
  glTranslatef(p[0], p[1], 0);
  rot=-rot*180/M_PI;
  glRotatef(rot+90,0,0,1);
  glColor3f(1.0,1.0,1.0);
  glEnable(GL_TEXTURE_2D);
  if (tex.isTransparent()) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  glBindTexture(GL_TEXTURE_2D,tex.getTextureID());
  glBegin(GL_QUADS);
  glTexCoord2f(0,1);
  glVertex2f(-w2,-h2);
  glTexCoord2f(1,1);
  glVertex2f(w2,-h2);
  glTexCoord2f(1,0);
  glVertex2f(w2,h2);
  glTexCoord2f(0,0);
  glVertex2f(-w2,h2);
  glEnd();
  if (tex.isTransparent()) {
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
  }
  glPopMatrix();
}

const std::vector<TeamStat> &
SDLGLGUI::getTeamStats() 
{
  return m_client.getGame().getTeamStat();
}

/*
void 
SDLGLGUI::drawTextRow(const std::string &text, bool centered)
{
  const std::vector<Team> &teams(m_client.getGame().getTeams());
  int xtiles=16,ytiles=16;
  int w=m_fontPtr->getWidth(),h=m_fontPtr->getHeight();
  float fw=w,fh=h;
  int dx=w/xtiles;
  int dy=h/ytiles;
  float fdx=1.0f/xtiles;
  float fdy=1.0f/ytiles;
  int offset=0;

  if (centered) {
    unsigned nc=text.size();
    unsigned ts=0;
    for (unsigned i=0;i<nc;++i) {
      if (text[i]>=32) ++ts;
    }
    offset=-int(ts)*dx/2;
  }
  
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D, m_fontPtr->getTextureID());
  glBegin(GL_QUADS);
  unsigned col=0;
  for (unsigned i=0;i<text.size();++i){
    if (text[i]>=32) {
      float cx=float((int(text[i]-32)%xtiles)*dx)/fw;
      float cy=float((int(text[i]-32)/xtiles)*dy)/fh;
      int x=col*dx+offset;
      glTexCoord2f(cx,cy);
      glVertex2i(x,dy);
      glTexCoord2f(cx+fdx,cy);
      glVertex2i(x+dx,dy);
      glTexCoord2f(cx+fdx,cy+fdy);
      glVertex2i(x+dx,0);
      glTexCoord2f(cx,cy+fdy);
      glVertex2i(x,0);
      ++col;
    }else if ((text[i]>=11)&&(text[i]<15)) {
      unsigned t=text[i]-11;
      t%=teams.size();
      assert(t<teams.size());
      glColor3f(teams[t].color[0],teams[t].color[1],teams[t].color[2]);
    }
  }
  glEnd();
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}

void 
SDLGLGUI::drawText(const std::string &text, bool centered)
{
  std::string::size_type pos(text.find_first_of('\n'));
  if (pos!=std::string::npos) {
    std::string row(text,0,pos);
    drawTextRow(row,centered);
    if (pos+1<text.size()) {
      int ytiles=16;
      glTranslatef(0,-m_fontPtr->getHeight()/ytiles,0);
      drawText(std::string(text,pos+1),centered);
    }
    return;
  }
  drawTextRow(text,centered);
}
*/

void
SDLGLGUI::drawPillars()
{
  glColor3f(1.0,1.0,1.0);
  // hand crafted drawTexture
  // drawTexture(*m_circlePtr,p);
  const Texture &tex(*m_circlePtr.get());
  int w=tex.getWidth();
  int h=tex.getHeight();
  int w2=w>>1;
  int h2=h>>1;
  
  
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,tex.getTextureID());

  glBegin(GL_QUADS);
  const Game::WorldPtr &worldPtr(m_client.getWorldPtr());
  unsigned np=worldPtr->getNumVertices();
  const std::vector<V2D> &v(worldPtr->getVertices());
  for (unsigned p=0;p<np;++p) {
    float x=v[p][0];
    float y=v[p][1];
    glTexCoord2f(0,1);
    glVertex2f(x-w2,y-h2);
    glTexCoord2f(1,1);
    glVertex2f(x+w2,y-h2);
    glTexCoord2f(1,0);
    glVertex2f(x+w2,y+h2);
    glTexCoord2f(0,0);
    glVertex2f(x-w2,y+h2);
  }
  glEnd();
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  flush();
}

void
SDLGLGUI::setupCamera(R dt)
{
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
	if (id<players.size()) {
	  pos+=players[id].m_pos;
	  /* did not compile on mingw gcc 3.1.?
	    minp[0]=std::min(minp[0],players[id].m_pos[0]);
	    minp[1]=std::min(minp[1],players[id].m_pos[1]);
	  */
#define min(a,b) (a<=b) ? a : b
	  minp[0]=min(minp[0],players[id].m_pos[0]);
	  minp[1]=min(minp[1],players[id].m_pos[1]);
#undef min

#define max(a,b) (a>=b) ? a : b
	  maxp[0]=max(maxp[0],players[id].m_pos[0]);
	  maxp[1]=max(maxp[1],players[id].m_pos[1]);
#undef max
	  maxr=std::max(maxr,players[id].m_r);
	  ++c;
	}
      }
    if (c) {
      if (m_autoCenter) {
	pos=(maxp+minp)/2;
	pos[0]=int(pos[0]);
	pos[1]=int(pos[1]);
	m_camera.setPos(pos+V2D(0.375f,0.375f));
      }
      if ((c>1)&&m_autoZoom) {
	maxp-=minp;
	R xzoom=double(m_width)/(maxp[0]+maxr*2.0+300.0);
	R yzoom=double(m_height)/(maxp[1]+maxr*2.0+300.0);
	m_camera.setZoom(std::min(std::min(xzoom,yzoom),R(2.0)));
      }
    }
  }
  if (m_scrollOp[0]||m_scrollOp[1]) {
    V2D s(m_scrollOp[0],m_scrollOp[1]);
    s*=dt*400/getZoom();
    m_camera.setPos(m_camera.getWPos()+s);
  }
  if (m_zoomOp) {
    m_camera.setZoom(std::max(R(m_camera.getWZoom()*(1.0+0.4*dt*R(m_zoomOp))),R(0.1)));
  }
  m_camera.step(dt);
  
  glTranslatef(-int(getPos()[0]*getZoom())+m_width/2,-int(getPos()[1]*getZoom())+m_height/2,0);
  glScalef(getZoom(),getZoom(),1);
}

void
SDLGLGUI::drawWalls()
{
  // TODO: this is really slow

  const Game::WorldPtr &worldPtr(m_client.getWorldPtr());
  unsigned wc=worldPtr->getNumWalls();
  if (!wc) return;
  
  // backup line width
  float lw;
  glGetFloatv(GL_LINE_WIDTH,&lw);

  int q=getGUIConfig().quality;
  float nlw;
  int numlines=3;

  // TODO: we assume all walls have the same width
  Wall w;
  worldPtr->getWall(0,w);
  float wt=w.getWallWidth();

  nlw=wt*getZoom()/float(numlines);
  if (m_lineSmooth) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
  }
  // now set the line width
  glLineWidth(nlw);


  for (unsigned wid=0;wid<wc;++wid)
    {
      if (worldPtr->getWall(wid,w)) {
	const FWEdge &e=worldPtr->getEdge(wid);
	bool cwClosed=m_client.getGame().roomIsClosed(e.m_rcw);
	bool ccwClosed=m_client.getGame().roomIsClosed(e.m_rccw);

	//drawWall(w,cwClosed,ccwClosed);

	if (q<=1) {
	  // TODO: test if this is really faster than
	  // always drawing two lines without setting a new line width
	  // especially with mesa - because this is the target for low
	  // quality mode

	  // in low quality mode we only draw 1 or 2 lines
	  numlines=(cwClosed==ccwClosed) ? 1 : 2;
	  nlw=wt*getZoom()/float(numlines);
	  glLineWidth(nlw);
	}
	
	Line l(w.getLine());
	const V2D &ln(l.normal());
	if (numlines>1) {
	  // draw clockwise side
	  V2D a(l.m_a);
	  V2D b(l.m_b);
	  float o=wt/numlines;
	  a+=ln*o;
	  b+=ln*o;
	  if (cwClosed) glColor3f(0.5,0.0,0.0);
	  else glColor3f(0.0,0.5,0.0);
	  glBegin(GL_LINES);
	  glVertex2f(a[0],a[1]);
	  glVertex2f(b[0],b[1]);
	  glEnd();
	  // draw counter clockwise side
	  a=l.m_a;
	  b=l.m_b;
	  a+=ln*-o;
	  b+=ln*-o;
	  if (ccwClosed) glColor3f(0.5,0.0,0.0);
	  else glColor3f(0.0,0.5,0.0);
	  glBegin(GL_LINES);
	  glVertex2f(a[0],a[1]);
	  glVertex2f(b[0],b[1]);
	  glEnd();
	}
	if (numlines!=2) {
	  // draw middle    
	  float hs=(numlines<3) ? 0.5f : 0.7f;
	  if (cwClosed&&ccwClosed) glColor3f(hs,0.0,0.0);
	  else if (cwClosed||ccwClosed) glColor3f(hs,0.4,0.0);
	  else glColor3f(0.0,hs,0.0);
	  V2D a(l.m_a);
	  V2D b(l.m_b);
	  glBegin(GL_LINES);
	  glVertex2f(a[0],a[1]);
	  glVertex2f(b[0],b[1]);
	  glEnd();
	}
      }
    }
  
  // restore linewidth
  glLineWidth(lw);
  if (m_lineSmooth) glDisable(GL_BLEND);
}

void
SDLGLGUI::drawDoors()
{
  const Game::WorldPtr &worldPtr(m_client.getWorldPtr());
  std::vector<FWEdge::EID> d(worldPtr->getAllDoors());
  Game::Doors &doors(m_client.getGame().getDoors());
  for (unsigned d=0;d<doors.size();++d) {
    bool closed=(doors[d].isClosed());
    RealDoor rd(m_client.getGame().doorInWorld(doors[d]));
    Wall w(rd.asWall());
    drawDoor(w,closed);
  }
}

void
SDLGLGUI::drawPlayers(R dt)
{
  const Game::Players &players(m_client.getPlayers());
  if (m_animations.size()!=players.size()) {
    while (players.size()<m_animations.size())
      m_animations.pop_back();
    while (m_animations.size()<players.size())
      {
	PlayerID id=m_animations.size();
	if (players[id].isPlayer()) {
	  // find team this player is in todo: similar code in game.cpp
	  unsigned tid=~0U;
	  const std::vector<Team> &teams(getTeams());
	  unsigned i=0;
	  for (;i<teams.size();++i) {
	    std::vector<PlayerID>::const_iterator it(std::find(teams[i].playerIDs.begin(),teams[i].playerIDs.end(),id));
	    if (it!=teams[i].playerIDs.end()) {
	      tid=it-teams[i].playerIDs.begin();
	      break;
	    }
	  }
	  if ((i>=teams.size())||(tid==~0U)||(tid>=teams[i].textures.size())) {
	    DOPE_WARN("report this as bug! Include: "<<i<<","<<teams.size()<<","<<tid);
	    std::vector<std::string> uris;
	    uris.push_back("data:unknown.png");
	    m_animations.push_back(Animation(*this,uris));
	  }else{
	    m_animations.push_back(Animation(*this,teams[i].textures[tid]));
	  }
	}else{
	  std::vector<std::string> uris;
	  switch (players[id].getType()) {
	  case 10:
	    uris.push_back("data:barrel.png");
	    break;
	  case 11:
	    for (unsigned i=1;i<=13;++i) {
	      std::ostringstream o;
	      o << std::setw(2) << std::setfill('0') << i;
	      uris.push_back(std::string("data:erfrischung_00")+o.str()+".png");
	    }
	    break;
	  default:
	    uris.push_back("data:unknown.png");
	    DOPE_WARN("Unknown type => unknown texture");
	  }
	  m_animations.push_back(Animation(*this,uris));
	}
      }
  }
  for (unsigned p=0;p<players.size();++p)
    {
      const Player &cp(players[p]);
      if (m_client.getGame().playerIsLocked(p))
	glColor3f(1.0,0.0,0.0);
      else
	glColor3f(0.0,1.0,0.0);
      if (players[p].isPlayer())
	m_animations[p].step(dt*cp.getY()*cp.getSpeed().length()*0.06);
      else
	m_animations[p].step(dt*20);
      drawTexture(m_animations[p].getTexture(),cp.m_pos,cp.getDirection());
      if (cp.isPlayer()&&m_showNames) {
	glPushMatrix();
	glTranslatef(int(cp.m_pos[0]), int(cp.m_pos[1])+2*cp.m_r, 0);
	float s=1.0f/getZoom();
	glScalef(s,s,1);
	m_fontPtr->drawText(m_client.getPlayerName(p),true);

	if (!(m_toggles&(1<<8))) {
	  // draw some info
	  glTranslatef(0,-40,0);
	  std::ostringstream o;
	  o.setf(std::ios::fixed);
	  o.precision(2);
	  o << "Room: " << m_client.getGame().playerInRoomCached(p) << " Pos:";
	  o << cp.m_pos[0] << "/" << cp.m_pos[1] << " Locked" << m_client.getGame().playerIsLocked(p);
	  m_fontPtr->drawText(o.str(),true);
	}

	glPopMatrix();
      }
    }
}

void
SDLGLGUI::drawTeamStat()
{
  // paint team statistics
  const std::vector<Team> &teams(getTeams());
  if (teams.size()) {
    const std::vector<TeamStat> &teamStat(getTeamStats());
    assert(teams.size()==teamStat.size());
    int dx=m_width/(teams.size()+1);
    glPushMatrix();
    if (teams.size()>m_fontPtr->numColors()) {
      for (unsigned i=0;i<teams.size();++i)
	m_fontPtr->setColor(i,teams[i].color);
    }
    for (unsigned i=0;i<teams.size();++i) {
      glLoadIdentity();
      glTranslatef(dx*(i+1),30,0);
      m_fontPtr->drawText(
	       m_fontPtr->getColor(i)
	       +teams[i].name+"\n"
	       +anyToString(teamStat[i].numPlayers-teamStat[i].locked)+"/"
	       +anyToString(teamStat[i].numPlayers),
	       true);
      flush();
    }
    glPopMatrix();
  }
}
