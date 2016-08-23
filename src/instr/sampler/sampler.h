#include "../../includes.h"
#include "../instr.h"

class Sampler: public OTrackInstrument {
  int period;
  public:
    OTrackInsSpec* getspec();
    float* getSample();
    bool init(int inChannels, int outChannels);
};
