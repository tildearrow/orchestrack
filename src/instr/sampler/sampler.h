#include "../../includes.h"
#include "../instr.h"
#include "../../font/font.h"

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
    float rate;
    float* data;
  };
  font* f;
  std::vector<voice> v;
  std::vector<smp> s;
  unsigned char* ev;
  public:
    OTrackInsSpec* getspec();
    float* getSample();
    void setRenderer(SDL_Renderer* renderer);
    void drawUI();
    bool init(int inChannels, int outChannels);
};
