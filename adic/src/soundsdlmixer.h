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
   \file soundsdlmixer.h
   \brief mikmod sound implementation
   \author Jens Thiele
*/

#ifndef SOUNDSDLMIXER_H
#define SOUNDSDLMIXER_H

#include "sound.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

//! sound implementation using SDL_mixer
class SoundSDLMixer : public Sound
{
protected:
  void deinit();

  int audio_open;
  Mix_Music *music;
  bool oldPlaying;

  typedef std::map<std::string, Mix_Chunk *> Samples;
  Samples samples;
public:
  
  SoundSDLMixer(SoundConfig &sc);
  
  ~SoundSDLMixer();

  void playMusic(const char *uri, R volume=R(1), unsigned repeat=0);
  void stopMusic(const char *uri);
  
  void step(R dt);
  
  void loadSample(const char *uri);
  int playSample(const char *uri, int repeat=0);
  void modifyChannel(int, R volume, R balance=R(0), R pitch=R(1));
  void stopChannel(int);
  void unloadSample(const char *uri);
};

#endif
