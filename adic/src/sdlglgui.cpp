#include "sdlglgui.h"


SDLGLGUI::SDLGLGUI(Client &client, const GUIConfig &config) 
  : GUI(client,config), m_terminal(*this,0,config.height-48),
    m_textureTime(5), 
    m_autoCenter(true), 
    m_zoom(1), m_zoomOp(0), m_autoZoom(true),
    m_showNames(true),
    m_quit(false), m_frames(0),
    m_chatMode(0)
{
  m_scrollOp[0]=m_scrollOp[1]=0;
  sf.quitSignal.connect(SigC::slot(*this,&SDLGLGUI::handleQuit));
  sf.resize.connect(SigC::slot(*this,&SDLGLGUI::handleResize));
  m_terminal.printed.connect(printed.slot());
  m_start.now();
}


bool
SDLGLGUI::init()
{
  // Initialize SDL
  if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) < 0 )
    throw std::runtime_error(std::string("Couldn't init SDL: ")+SDL_GetError());
  if (SDL_GL_LoadLibrary(m_config.libGL.c_str())==-1)
    throw std::runtime_error(std::string("Could not load OpenGL lib: \"")+m_config.libGL.c_str()+"\": "+SDL_GetError());
  gl.init();

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

  createWindow();
  return true;
}

void
SDLGLGUI::createWindow() 
{
  m_flags = SDL_OPENGL|SDL_RESIZABLE;
  if ( m_config.fullscreen) m_flags |= SDL_FULLSCREEN;

  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  SDL_ResizeEvent e;
  e.w=m_config.width;
  e.h=m_config.height;
  sf.resize.emit(e);
  //  resize(m_config.width, m_config.height);
  m_texturePtr=DOPE_SMARTPTR<Texture>(new Texture(gl,"data/textures.png"));
  m_fontTexPtr=DOPE_SMARTPTR<Texture>(new Texture(gl,"data/font.png"));
  m_fontPtr=DOPE_SMARTPTR<GLFont>(new GLFont(gl,m_fontTexPtr));
  m_circlePtr=DOPE_SMARTPTR<Texture>(new Texture(gl,"data/pillar.png"));
  m_texCircle=true;
  gl.Disable(GL_NORMALIZE);
  gl.Disable(GL_LIGHTING);
  gl.Disable(GL_CULL_FACE);
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
  gl.Viewport(0,0,width,height);
  // Select The Projection Matrix
  gl.MatrixMode(GL_PROJECTION);		
  gl.LoadIdentity();
  gl.Ortho(0.0f,width,0.0f,height,-100.0f,100.0f);
  gl.ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
  DOPE_SMARTPTR<Texture> r(new Texture(gl,uri.c_str()));
  m_textures[uri]=r;
  return r;
}

bool
SDLGLGUI::handleKey(SDL_KeyboardEvent e)
{
  bool pressed=e.state==SDL_PRESSED;
  SDLKey k(e.keysym.sym);
  if (!m_chatMode) {
    switch (k) {
    case SDLK_LEFT:
      m_scrollOp[0]=(pressed ? -1 : 0);
      m_autoCenter=false;
      return true;
    case SDLK_RIGHT:
      m_scrollOp[0]=(pressed ? 1 : 0);
      m_autoCenter=false;
      return true;
    case SDLK_UP:
      m_scrollOp[1]=(pressed ? 1 : 0);
      m_autoCenter=false;
      return true;
    case SDLK_DOWN:
      m_scrollOp[1]=(pressed ? -1 : 0);
      m_autoCenter=false;
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
    case SDLK_q:
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
      m_chatMode=1;
      SDL_EnableUNICODE(1);
      return true;
    case SDLK_g:
      m_chatMode=2;
      SDL_EnableUNICODE(1);
      return true;
    case SDLK_RETURN:
      if (!pressed)
	return true;
      break;
    case SDLK_c:
      if (pressed) m_texCirle=!m_texCircle;
    }
    return false;
  }
  // else chatMode
  if (!pressed) return true;

  // check for special keys
  switch (k) {
  case SDLK_RETURN:
    {
      SDL_EnableUNICODE(0);
      // we only fill the message and public field 
      // the server fills the other fields
      ChatMessage m;
      m.message=m_chatLine;
      m.global=(m_chatMode==1);
      chatMessage.emit(m);
      m_chatMode=0;
      m_chatLine.clear();
      printed.emit('\n');
      return true;
    }
    break;
  case SDLK_DELETE:
  case SDLK_BACKSPACE:
  case SDLK_LEFT:
    unsigned s=m_chatLine.size();
    if (s)
      m_chatLine.resize(s-1);
    printed.emit(k);
    return true;
  }

  Uint16 unicode(e.keysym.unicode);
  char ch;
  if ( (unicode & 0xFF80) == 0 ) {
    ch = unicode & 0x7F;
    if (ch>=32) {
      m_chatLine+=ch;
      printed.emit(ch);
    }
  }
  else {
    std::cerr << "An International Character.\n";
  }
  return true;
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


bool
SDLGLGUI::step(R dt)
{
  if (m_quit) return false;
  sf.produce();

  // Clear The Screen And The Depth Buffer
  //  gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl.Clear(GL_COLOR_BUFFER_BIT);
  // Reset The View
  gl.MatrixMode(GL_MODELVIEW);
  gl.LoadIdentity();
  gl.Color3f(1.0,1.0,1.0);

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
	pos=(maxp+minp)/2;
	pos[0]=int(pos[0]);
	pos[1]=int(pos[1]);
	m_pos=pos;
      }
      if ((c>1)&&m_autoZoom) {
	maxp-=minp;
	R xzoom=double(m_width)/(maxp[0]+maxr*2.0+300.0);
	R yzoom=double(m_height)/(maxp[1]+maxr*2.0+300.0);
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
  gl.Translatef(-int(m_pos[0]*m_zoom)+m_width/2,-int(m_pos[1]*m_zoom)+m_height/2,0);
  gl.Scalef(m_zoom,m_zoom,1);

  // paint world
  Game::WorldPtr worldPtr=m_client.getWorldPtr();
  if (worldPtr.get()) {
    // paint rooms
    for (unsigned r=0;r<worldPtr->getNumRooms();++r)
      {
	// select room color
	bool adic=m_client.getGame().roomIsClosed(r);
	if (adic)
	  gl.Color3f(0.5,0.0,0.0);
	else
	  gl.Color3f(0.0,0.5,0.0);
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
	    gl.Color3f(0.5,0.0,0.0);
	  else
	    gl.Color3f(0.0,0.5,0.0);
	  drawWall(w);
	}
      }
    // paint doors
    std::vector<FWEdge::EID> d(worldPtr->getAllDoors());
    Game::Doors &doors(m_client.getGame().getDoors());
    for (unsigned d=0;d<doors.size();++d) {
      if (doors[d].isClosed())
	gl.Color3f(0.5,0.0,0.0);
      else
	gl.Color3f(0.0,0.0,1.0);
      RealDoor rd(m_client.getGame().doorInWorld(doors[d]));
      Wall w(rd.asWall());
      drawWall(w);
    }
    gl.Color3f(1.0,1.0,1.0);    
  }else{
    DOPE_WARN("Did not receive world yet");
  }
  // paint players
  if (m_animations.size()!=players.size()) {
    while (players.size()<m_animations.size())
      m_animations.pop_back();
    while (m_animations.size()<players.size())
      {
	PlayerID id=m_animations.size();
	if (players[id].isPlayer()) {
	  // find team this player is in todo: similar code in game.cpp
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
	  m_animations.push_back(Animation(*this,teams[i].textures[tid]));
	}else{
	  std::vector<std::string> uris;
	  switch (players[id].getType()) {
	  case 10:
	    uris.push_back("data/barrel.png");
	    break;
	  case 11:
	    for (unsigned i=1;i<=13;++i) {
	      std::ostringstream o;
	      o << std::setw(2) << std::setfill('0') << i;
	      uris.push_back(std::string("data/erfrischung_00")+o.str()+".png");
	    }
	    break;
	  default:
	    DOPE_WARN("Unknown type => no texture");
	  }
	  m_animations.push_back(Animation(*this,uris));
	}
      }
  }
  for (unsigned p=0;p<players.size();++p)
    {
      const Player &cp(players[p]);
      if (m_client.getGame().playerIsLocked(p))
	gl.Color3f(1.0,0.0,0.0);
      else
	gl.Color3f(0.0,1.0,0.0);
      //      drawCircle(cp.m_pos,cp.m_r);
      if (players[p].isPlayer())
	m_animations[p].step(dt*cp.getY()*cp.getSpeed().length()*0.1);
      else
	m_animations[p].step(dt*20);
      drawTexture(m_animations[p].getTexture(),cp.m_pos,cp.getDirection());
      if (cp.isPlayer()&&m_showNames) {
	gl.PushMatrix();
	gl.Translatef(int(cp.m_pos[0]), int(cp.m_pos[1])+2*cp.m_r, 0);
	float s=1.0f/m_zoom;
	gl.Scalef(s,s,1);
	m_fontPtr->drawText(m_client.getPlayerName(p),true);
	gl.PopMatrix();
      }
      /*      V2D dv(V2D(0,100).rot(cp.getDirection()));
      gl.Color3f(1.0,1.0,0.0);
      gl.Begin(GL_LINES);
      gl.Vertex2f(cp.m_pos[0],cp.m_pos[1]);
      dv+=cp.m_pos;
      gl.Vertex2f(dv[0],dv[1]);
      gl.End(); 
      gl.Color3f(1.0,1.0,1.0); */
    }
  // paint team statistics
  const std::vector<Team> &teams(m_client.getGame().getTeams());
  if (teams.size()) {
    const std::vector<TeamStat> &teamStat(m_client.getGame().getTeamStat());
    int dx=m_width/(teams.size()+1);
    gl.PushMatrix();
    if (teams.size()>m_fontPtr->numColors()) {
      for (unsigned i=0;i<teams.size();++i)
	m_fontPtr->setColor(i,teams[i].color);
    }
    for (unsigned i=0;i<teams.size();++i) {
      gl.LoadIdentity();
      gl.Translatef(dx*(i+1),30,0);
      m_fontPtr->drawText(
	       m_fontPtr->getColor(i)
	       +teams[i].name+"\n"
	       +anyToString(teamStat[i].numPlayers-teamStat[i].locked)+"/"
	       +anyToString(teamStat[i].numPlayers),
	       true);
      gl.Flush();
    }
    gl.PopMatrix();
  }
  
  
  // paint texture
  if (m_textureTime>0) {
    gl.LoadIdentity();
    gl.Color4f(1.0,1.0,1.0,1.0-fabs(m_textureTime-2.5)/2.5);
    m_textureTime-=dt;
    gl.Enable(GL_TEXTURE_2D);
    gl.Enable(GL_BLEND);
    gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl.BindTexture(GL_TEXTURE_2D,m_texturePtr->getTextureID());
    gl.Begin(GL_QUADS);
    gl.TexCoord2f(0,1);
    gl.Vertex2f(0,0);
    gl.TexCoord2f(1,1);
    gl.Vertex2f(m_width,0);
    gl.TexCoord2f(1,0);
    gl.Vertex2f(m_width,m_height);
    gl.TexCoord2f(0,0);
    gl.Vertex2f(0,m_height);
    gl.End();
    gl.Disable(GL_BLEND);
    gl.Disable(GL_TEXTURE_2D);

  }
  // paint FPS
  TimeStamp ct;
  ct.now();
  ct-=m_start;
  R uptime=R(ct.getSec())+(R(ct.getUSec())/1000000);
  std::ostringstream o;
  ++m_frames;
  o << "Up: " << std::fixed << std::setprecision(2) << std::setw(7) << uptime 
    << "\nFPS: " << std::setw(6) << R(m_frames)/uptime;
  //	      << " Frame: " << std::setw(10) << m_frames;
  gl.LoadIdentity();
  gl.Translatef(0,m_height-m_fontPtr->getHeight(),0);
  gl.Color3f(1.0,1.0,1.0);
  m_fontPtr->drawText(o.str());

  // paint terminal
  m_terminal.step(dt);
  // paint chat line
  gl.LoadIdentity();
  gl.Translatef(0,0,0);
  gl.Color3f(1.0,1.0,1.0);
  m_fontPtr->drawText(m_chatLine);
  
  gl.Flush();
  gl.Finish();
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
  if (!m_texCircle) {
    unsigned d=1.0/m_zoom;
    unsigned res=12;
    if (d>1) res/=d;
    if (res>=3) {
      gl.PushMatrix();
      gl.Translatef(p[0], p[1], 0);
      gl.Begin(GL_TRIANGLE_FAN);
      gl.Vertex2f(0, 0);
      for (unsigned i=0;i<res;++i) {
	double angle(double(i)*2*M_PI/double(res-1));
	gl.Vertex2f(r * cos(angle), r * sin(angle));
      }
      gl.End();
      gl.PopMatrix();
      gl.Flush();
    }
  }
  else
    {
      drawTexture(m_circlePtr->getTexture(),p);
    }
}
void
SDLGLGUI::drawWall(const Wall &w)
{
  float lw;
  gl.GetFloatv(GL_LINE_WIDTH,&lw);
  gl.LineWidth(2*w.getWallWidth()*m_zoom);
  Line l(w.getLine());
  drawCircle(l.m_a,w.getPillarRadius());
  drawCircle(l.m_b,w.getPillarRadius());
  gl.Begin(GL_LINES);
  gl.Vertex2f(l.m_a[0],l.m_a[1]);
  gl.Vertex2f(l.m_b[0],l.m_b[1]);
  gl.End();
  gl.LineWidth(lw);
  gl.Flush();
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
  gl.PushMatrix();
  gl.Translatef(p[0], p[1], 0);
  rot=-rot*180/M_PI;
  gl.Rotatef(rot+90,0,0,1);
  gl.Color3f(1.0,1.0,1.0);
  gl.Enable(GL_TEXTURE_2D);
  gl.Enable(GL_BLEND);
  gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  gl.BindTexture(GL_TEXTURE_2D,tex.getTextureID());
  gl.Begin(GL_QUADS);
  gl.TexCoord2f(0,1);
  gl.Vertex2f(-w2,-h2);
  gl.TexCoord2f(1,1);
  gl.Vertex2f(w2,-h2);
  gl.TexCoord2f(1,0);
  gl.Vertex2f(w2,h2);
  gl.TexCoord2f(0,0);
  gl.Vertex2f(-w2,h2);
  gl.End();
  gl.Disable(GL_BLEND);
  gl.Disable(GL_TEXTURE_2D);
  gl.PopMatrix();
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
  
  gl.Enable(GL_TEXTURE_2D);
  gl.Enable(GL_BLEND);
  gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  gl.BindTexture(GL_TEXTURE_2D, m_fontPtr->getTextureID());
  gl.Begin(GL_QUADS);
  unsigned col=0;
  for (unsigned i=0;i<text.size();++i){
    if (text[i]>=32) {
      float cx=float((int(text[i]-32)%xtiles)*dx)/fw;
      float cy=float((int(text[i]-32)/xtiles)*dy)/fh;
      int x=col*dx+offset;
      gl.TexCoord2f(cx,cy);
      gl.Vertex2i(x,dy);
      gl.TexCoord2f(cx+fdx,cy);
      gl.Vertex2i(x+dx,dy);
      gl.TexCoord2f(cx+fdx,cy+fdy);
      gl.Vertex2i(x+dx,0);
      gl.TexCoord2f(cx,cy+fdy);
      gl.Vertex2i(x,0);
      ++col;
    }else if ((text[i]>=11)&&(text[i]<15)) {
      unsigned t=text[i]-11;
      t%=teams.size();
      assert(t<teams.size());
      gl.Color3f(teams[t].color[0],teams[t].color[1],teams[t].color[2]);
    }
  }
  gl.End();
  gl.Disable(GL_BLEND);
  gl.Disable(GL_TEXTURE_2D);
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
      gl.Translatef(0,-m_fontPtr->getHeight()/ytiles,0);
      drawText(std::string(text,pos+1),centered);
    }
    return;
  }
  drawTextRow(text,centered);
}
*/
