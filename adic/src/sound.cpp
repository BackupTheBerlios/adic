#include "sound.h"
#include "soundsdlmixer.h"

Sound* 
Sound::create(SoundConfig &sc)
{
  if (sc.sdriver=="sdlmixer")
    return new SoundSDLMixer(sc);
  return NULL;
}

