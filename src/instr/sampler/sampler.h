#include "../../includes.h"
#include "../instr.h"
#include "../../font/font.h"

class Sampler: public OTrackInstrument {
  struct voice {
    float period;
    float f;
    float vol;
  };
  font* f;
  std::vector<voice> v;
  unsigned char* ev;
  public:
    OTrackInsSpec* getspec();
    float* getSample();
    void setRenderer(SDL_Renderer* renderer);
    void drawUI();
    bool init(int inChannels, int outChannels);
};
