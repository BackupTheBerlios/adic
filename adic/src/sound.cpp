#include "sound.h"
#include "soundsdlmixer.h"

Sound* 
Sound::create(SoundConfig &sc)
{
  if (sc.sdriver=="sdlmixer") {
    // todo - I am not sure if this is correct
    SoundSDLMixer* n=NULL;
    try{
      n=new SoundSDLMixer(sc);
    }catch(const std::runtime_error &error){
      std::cerr << error.what() << "\nsound disabled\n";
      delete n;
      n=NULL;
    }
    return new n;
  }
  return NULL;
}

