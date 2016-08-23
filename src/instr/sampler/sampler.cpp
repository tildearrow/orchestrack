#include "sampler.h"

OTrackInsSpec* OTrackInstrument::getspec() {
  OTrackInsSpec* aspec;
  aspec=new OTrackInsSpec;
  aspec->name="Sampler";
  aspec->desc="default sampler";
  aspec->author="tildearrow";
  aspec->copyright="none";
  aspec->license="none";
  return aspec;
}

void getSample(int channels, float *sample, ...) {
  va_list va;
  va_start(va,sample);
  
}