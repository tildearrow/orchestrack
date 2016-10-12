#include "sampler.h"

void Sampler::vErase(size_t which) {
  memcpy(&v[which],&v[which+1],sizeof(voice)*(vSize-which-1));
  vResize(vSize-1);
}

void Sampler::pErase(envp** which, size_t* cursize, size_t which1) {
  //printf("erasing!!\n");
  memcpy(&which[0][which1],&which[0][which1+1],sizeof(envp)*((*cursize)-which1-1));
  pResize(which,cursize,(*cursize)-1);
}
