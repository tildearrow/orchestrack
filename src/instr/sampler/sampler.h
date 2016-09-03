#include "../../includes.h"
#include "../instr.h"
#include "../../font/font.h"
#include "../../ui/button.h"
#include <sndfile.h>

class Sampler: public OTrackInstrument {
  struct voice {
    int chan;
    int note;
    int pos;
    float period;
    float f;
    float vol;
  };
  struct smp {
    int len;
    int chan;
    float rate;
    float* data;
  };
  font* f;
  std::vector<voice> v;
  std::vector<smp> s;
  unsigned char* ev;
  SNDFILE* sndf;
  SF_INFO si;
  public:
    OTrackInsSpec* getspec();
    float* getSample();
    void setRenderer(SDL_Renderer* renderer);
    void drawUI();
    bool init(int inChannels, int outChannels);
};
