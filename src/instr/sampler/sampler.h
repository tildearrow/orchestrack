#include "../../includes.h"
#include "../instr.h"

class Sampler: public OTrackInstrument {
  struct voice {
    float period;
    float f;
    float vol;
  };
  std::vector<voice> v;
  unsigned char* ev;
  public:
    OTrackInsSpec* getspec();
    float* getSample();
    bool init(int inChannels, int outChannels);
};
