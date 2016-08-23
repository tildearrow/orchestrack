#include "sampler.h"

OTrackInsSpec* Sampler::getspec() {
  OTrackInsSpec* aspec;
  aspec=new OTrackInsSpec;
  aspec->name="Sampler";
  aspec->desc="default sampler";
  aspec->author="tildearrow";
  aspec->copyright="none";
  aspec->license="none";
  return aspec;
}

float* Sampler::getSample() {
  sample[0]=(float)(period%256)/256.0;
  sample[1]=(float)(period%256)/256.0;
  period++;
  return sample;
}

bool Sampler::init(int inChannels, int outChannels) {
  if (inChannels==0) {
    sample=new float[outChannels];
    outCh=outChannels;
    period=0;
    return true;
  } else {
    return false;
  }
}
