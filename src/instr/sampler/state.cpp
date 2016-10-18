#include "sampler.h"

bool Sampler::loadState(FILE* data) {
  // for now display an error
  erra=64;
  errl[0]=friendlyErr0(0);
  errl[1]=friendlyErr1(0);
  errl[2]=friendlyErr2(0);
  return false;
}

unsigned char* Sampler::saveState(int* size) {
  return NULL;
}
