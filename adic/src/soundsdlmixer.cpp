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
   \file soundsdlmixer.cpp
   \brief SDL_mixer sound implementation
   \author Jens Thiele
*/

#include "soundsdlmixer.h"
SoundSDLMixer::SoundSDLMixer(SoundConfig &sc)
  : Sound(sc), audio_open(0), music(NULL), oldPlaying(false)
{
  if ( SDL_InitSubSystem(SDL_INIT_AUDIO) < 0 ) 
    throw std::runtime_error(std::string("Couldn't initialize SDL: ")+SDL_GetError());
    
  /* Open the audio device */
  int audio_rate = m_sc.srate;
  Uint16 audio_format=AUDIO_S8;
  if (m_sc.sbits==16)
    audio_format=AUDIO_S16;
  int audio_channels=1;
  if (m_sc.stereo) audio_channels=2;
  int audio_buffers = m_sc.sbuffers;

  if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) < 0) {
    throw std::runtime_error(std::string("Couldn't open audio: ")+SDL_GetError());
  } else {
    Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
    printf("Opened audio at %d Hz %d bit %s, %d bytes audio buffer\n", audio_rate,
	   (audio_format&0xFF),
	   (audio_channels > 1) ? "stereo" : "mono", 
	   audio_buffers );
  }
  audio_open = 1;
  
  /* Set the external music player, if any */
  Mix_SetMusicCMD(getenv("MUSIC_CMD"));
  
}

SoundSDLMixer::~SoundSDLMixer()
{
  deinit();
}

void SoundSDLMixer::deinit()
{
  stopMusic(NULL);
  for (Samples::iterator it=samples.begin();it!=samples.end();++it)
    {
      unloadSample(it->first.c_str());
    }
  if ( audio_open ) {
    Mix_CloseAudio();
    audio_open = 0;
  }
}

void
SoundSDLMixer::playMusic(const char *uri, R volume, unsigned repeat) 
{
  /* Load the requested music file */
  music = Mix_LoadMUS(uri);
  if ( music == NULL )
    throw std::runtime_error(std::string("Couldn't load ")+uri+" "+SDL_GetError());
  Mix_FadeInMusic(music,0,2000);
}
  
void
SoundSDLMixer::stopMusic(const char *uri) 
{
  if( Mix_PlayingMusic() ) {
    Mix_FadeOutMusic(1500);
    SDL_Delay(1500); // todo - this is impossible !!
  }
  if ( music ) {
    Mix_FreeMusic(music);
    music = NULL;
  }
}
  
void 
SoundSDLMixer::step(R dt) 
{
  int playing=Mix_PlayingMusic();
  if ((!playing)&&(oldPlaying))
    musicFinished.emit();
}
  
void
SoundSDLMixer::loadSample(const char *uri) 
{
  if (samples.find(std::string(uri))!=samples.end())
    return;
  samples[uri]=Mix_LoadWAV(uri);
}
  
int
SoundSDLMixer::playSample(const char * uri)
{
  loadSample(uri);
  return Mix_PlayChannel(-1,samples[uri],0);
}

void 
SoundSDLMixer::modifySample(const char * sampleID, R volume, R balance, unsigned repeat, R pitch) {}

void
SoundSDLMixer::modifyChannel(int channel, R volume, R balance, unsigned repeat, R pitch)
{
  Mix_Volume(channel, int(volume*128));
}

void 
SoundSDLMixer::stopChannel(int channel) 
{
}

void 
SoundSDLMixer::unloadSample(const char * uri) 
{
  Samples::iterator it(samples.find(uri));
  if (it!=samples.end())
    Mix_FreeChunk(it->second);
}
