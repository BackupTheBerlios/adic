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
   \file sdlsigfactory.h
   \brief class SDLSigFactory
   \author Jens Thiele
*/

#ifndef SDLSIGFACTORY_H
#define SDLSIGFACTORY_H

#include <SDL.h>
#include <sigc++/signal_system.h>

//! sig c++ marshaller which stops emit on first true value
struct StopOnTrue
{
  typedef bool InType;
  typedef bool OutType;
  OutType  return_value_;
  
  OutType value() { return return_value_; }
  static OutType default_value() { return false; }
  bool marshal(const InType& val) { return_value_ = val; return val; }
  
  StopOnTrue() : return_value_(false) {}
};


class SDLSigFactory {
public:
  typedef SigC::Signal0<void> QuitSignal;
  typedef SigC::Signal1<void, SDL_MouseMotionEvent> MouseMotion;
  typedef SigC::Signal1<void, SDL_MouseButtonEvent> MouseButton;
  typedef SigC::Signal1<bool, SDL_KeyboardEvent, StopOnTrue> KeyEvent;
  typedef SigC::Signal1<void, SDL_JoyAxisEvent> JoyMotion;
  typedef SigC::Signal1<void, SDL_JoyButtonEvent> JoyButton;
  typedef SigC::Signal1<void, float> TimeStep;
  typedef SigC::Signal1<void, SDL_ResizeEvent> ResizeSignal;

  QuitSignal quitSignal;
  MouseMotion mouseMotion;
  MouseButton mouseButton;
  KeyEvent keyEvent;
  JoyMotion joyMotion;
  JoyButton joyButton;
  ResizeSignal resize;

  void produce() {
    SDL_Event event;
    while ( SDL_PollEvent(&event) ) {
      switch (event.type) {
      case SDL_QUIT:
	quitSignal.emit();
	break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
	keyEvent.emit(event.key);
	break;
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
      case SDL_VIDEORESIZE:
	resize.emit(event.resize);
	break;
      default:
	break;
      }
    }
  }
};

#endif

