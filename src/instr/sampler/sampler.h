#include "../../includes.h"
#include "../instr.h"

class Sampler: public OTrackInstrument {
  float period;
  float f;
  unsigned char* ev;
  public:
    OTrackInsSpec* getspec();
    float* getSample();
    bool init(int inChannels, int outChannels);
};
