
/*
 * Copyright (C) 2002 Jens Thiele <karme@berlios.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*!
   \file sdlglgui.h
   \brief gui implementation using SDL and OpenGL
   \author Jens Thiele
*/

#ifndef SDLGLGUI_H
#define SDLGLGUI_H

#include "gui.h"
#include "texture.h"
#include "sdlsigfactory.h"
#include "sdlinputdev.h"
#include "animation.h"
#include "glterminal.h"
#include "glfont.h"
#include "sdlinputfield.h"
#include "glpoly.h"
#include "camera.h"

class SDLMenu;

//! currently the only GUI implementation. It uses SDL with OpenGL/Mesa
class SDLGLGUI : public GUI
{
public:
  SDLGLGUI(Client &client);

  //! intitialize gui
  bool init();
  //! repaint
  bool step(R dt);

  //! center of screen coordinates
  V2D getPos() const
  {
    return m_camera.getPos();
  }
  
  //! current zoom
  R getZoom() const 
  {
    return m_camera.getZoom();
  }

  //! current zoom
  R getRotate() const 
  {
    return m_camera.getRotate();
  }
  
  //! cleanup
  ~SDLGLGUI();

  bool handleKey(SDL_KeyboardEvent e);
  void handleResize(SDL_ResizeEvent e);
  void handleQuit();
  void handleChatInput(const std::string &msg);
  void handleNewClient(DOPE_SMARTPTR<NewClient> mPtr);
  void handleGreeting(DOPE_SMARTPTR<ServerGreeting> gPtr);
  void handleEndGame(DOPE_SMARTPTR<EndGame> egPtr);
  
  //! load a texture (or fetch it from the cache)
  DOPE_SMARTPTR<Texture> getTexture(const std::string &uri);

  //! simple terminal
  GLTerminal m_terminal;
  //! our font
  DOPE_SMARTPTR<GLFont> m_fontPtr;

  std::ostream &getOstream() 
  {
    return m_terminal.out;
  }
  unsigned numInputDevices() const
  {
    return m_inputDevices.size();
  }

  void drawCircle(const V2D &p, float r);
  void drawWall(const Wall &wall, bool cwClosed=false, bool ccwClosed=false);
  void drawDoor(const Wall &wall, bool closed=false);
  void drawTexture(const Texture &tex, const V2D &p, R rot=0);

  //! return width - Attention: may change during the game
  int getWidth() const
  {
    return m_width;
  }
  //! return height - Attention: may change during the game
  int getHeight() const
  {
    return m_height;
  }

  //! get signal factory
  SDLSigFactory &getSigFactory()
  {
    return sf;
  }

  //! get current teams
  const std::vector<Team> &getTeams() const
  {
    return m_client.getGame().getTeams();
  }
  
  //! get current team statistics
  const std::vector<TeamStat> &getTeamStats();

  //! flush GL command buffers (depending on the configuration)
  void flush() 
  {
    if (!getGUIConfig().flush) return;
    glFlush();
  }
protected:
  void drawPillars();
  void drawPolys();
  void setupCamera(R dt);
  void drawWalls();
  void drawDoors();
  void drawPlayers(R dt);
  void drawTeamStat();
  
  //! create window
  void createWindow();
  //! resize gui
  void resize(int width, int height);
  //! close window
  /*!
    \note after you called this you must not use any GL/GLU calls
  */
  void killWindow();
  
  std::vector<DOPE_SMARTPTR<SDLInputDev> > m_inputDevices;

  int m_width;
  int m_height;
  unsigned m_flags;
  //  DOPE_SMARTPTR<Texture> m_texturePtr;
  //  R m_textureTime;
  DOPE_SMARTPTR<Texture> m_fontTexPtr;
  std::vector<Animation> m_animations;
  DOPE_SMARTPTR<Texture> m_circlePtr;
  bool m_texCircle;
  std::vector<DOPE_SMARTPTR<Texture> > m_roomTextures;

  //! our camera
  /*!
    \note here we are using degrees and a left hand coordinate system
  */
  Camera m_camera;
  
  int m_scrollOp[2];
  bool m_autoCenter;

  int m_zoomOp;
  bool m_autoZoom;

  int m_rotateOp;
  bool m_autoRotate;
  
  typedef std::map<std::string, DOPE_SMARTPTR<Texture> > Textures;
  Textures m_textures;
  bool m_showNames;

  SDLSigFactory sf;
  bool m_quit;

  TimeStamp m_start;
  unsigned m_frames;

  //! global or group chat ?
  int m_chatMode;
  SDLInputField m_chatLine;

  //! the menu
  DOPE_SMARTPTR<SDLMenu> m_menuPtr;

  //! line smoothing ?
  bool m_lineSmooth;

  //! some toggles
  unsigned m_toggles;

  //! room polygons
  std::vector<DOPE_SMARTPTR<GLPoly> > m_polys;

  //! have world
  bool m_haveWorld;
};

#endif
