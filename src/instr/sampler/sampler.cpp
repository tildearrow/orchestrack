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
  ev=(unsigned char*)getEvent();
  if (ev!=NULL) {
    if ((ev[0]>>4)==9) {
      f=440*pow(2,((float)ev[1]-69)/12);
    }
  }
  sample[0]=(float)period;
  sample[1]=(float)period;
  period=fmod(period+f/44100,1);
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
