/*
 * Copyright (C) 2002 Jens Thiele <jens.thiele@student.uni-tuebingen.de>
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

class SDLGLGUI : public GUI
{
public:
  SDLGLGUI(Client &client, const GUIConfig &config) ;

  //! intitialize gui
  bool init();
  //! repaint
  bool step(R dt);
  //! center of screen coordinates
  V2D getPos() const;
  //! cleanup
  ~SDLGLGUI(){m_textures.clear();killWindow();}

  bool handleKey(SDL_KeyboardEvent e);
  void handleResize(SDL_ResizeEvent e);
  void handleQuit();

  //! load a texture (or fetch it from the cache)
  DOPE_SMARTPTR<Texture> getTexture(const std::string &uri);

  SDLGL gl;
  GLTerminal m_terminal;
  DOPE_SMARTPTR<GLFont> m_fontPtr;

  std::ostream &getOstream() 
  {
    return m_terminal.out;
  }
  unsigned numInputDevices() const
  {
    return m_inputDevices.size();
  }
protected:
  //! create window
  void createWindow();
  //! resize gui
  void resize(int width, int height);
  //! close window
  void killWindow();

  void drawCircle(const V2D &p, float r);
  void drawWall(const Wall &wall);
  void drawPolygon(const std::vector<V2D> &p);
  void drawTexture(const Texture &tex, const V2D &p, R rot=0);
  
  std::vector<DOPE_SMARTPTR<SDLInputDev> > m_inputDevices;

  int m_width;
  int m_height;
  unsigned m_flags;
  DOPE_SMARTPTR<Texture> m_texturePtr;
  R m_textureTime;
  DOPE_SMARTPTR<Texture> m_fontTexPtr;
  std::vector<Animation> m_animations;
  DOPE_SMARTPTR<Texture> m_circlePtr;
  bool m_texCircle;
  
  V2D m_pos;
  int m_scrollOp[2];
  bool m_autoCenter;

  R m_zoom;
  int m_zoomOp;
  bool m_autoZoom;

  
  typedef std::map<std::string, DOPE_SMARTPTR<Texture> > Textures;
  Textures m_textures;
  bool m_showNames;

  SDLSigFactory sf;
  bool m_quit;

  TimeStamp m_start;
  unsigned m_frames;

  int m_chatMode;
  std::string m_chatLine;
};

#endif
