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
   \file sound.h
   \brief Sound Interface
   \author Jens Thiele
*/

#ifndef SOUND_H
#define SOUND_H

#include "soundconfig.h"
#include <sigc++/signal_system.h>

//! sound interface class
/*!
  simple sound interface
  which is capable of playing one music file and a possible infinite
  number of samples - an implementation should allow at least 4 samples to be played
*/
class Sound
{
protected:
  //! sound configuration
  SoundConfig &m_sc;
public:
  Sound(SoundConfig &sc) : m_sc(sc)
  {}
  virtual ~Sound()
  {}

  static Sound* create(SoundConfig &sc);

  //! play a music file in the background
  /*!
    if another song is still playing it will be stopped
    (a nice implementation will fade out) and the new song ist started)

    \param volume (0<=volume<=1 with 0=silent 1=maximum)
    \param repeat how often we should play this file
  */
  virtual void playMusic(const char *uri, R volume=R(1), unsigned repeat=0)=0;
  
  virtual void stopMusic(const char *uri)=0;

  //! signal that is emitted if music is finished
  SigC::Signal0<void> musicFinished;
  
  //! this is called every frame
  /*!
    This should allow an implementation without threads

    \param dt time elapsed since last call
  */
  virtual void step(R dt)=0;
  
  //! load a sample
  /*!
    \param uri the URI of the sample
  */
  virtual void loadSample(const char *uri)=0;
  
  //! play a previously loaded sample
  /*!
    \return the channel number the sample is played on
  */
  virtual int playSample(const char *uri, int repeat=0)=0;

  //! modify channel settings
  /*!
    \param volume the volume setting (0<=volume<=1 with 0=silent 1=maximum)
    \param balance left right balance setting (-1<=balance<=1 with -1=left +1=right)
    \param pitch pitch factor - a simple implementation will only modify the speed
  */
  virtual void modifyChannel(int channel, R volume, R balance=R(0), R pitch=R(1))=0;
  
  //! stop playing of sample
  virtual void stopChannel(int channel)=0;

  virtual void unloadSample(const char *uri)=0;
};

#endif
