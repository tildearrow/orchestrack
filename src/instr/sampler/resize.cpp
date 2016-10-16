#include "sampler.h"

void Sampler::vResize(size_t newsize) {
  if (newsize>255) {
    printf("trying to play more than 255 voices!\n");
    vErase(0);
    return;
  }
  if (newsize>vSize) {
    //printf("set\n");
    for (size_t i=vSize; i<newsize; i++) {
      //printf("seeet\n");
      memset(&v[i],0,sizeof(voice));
    }
  }
  vSize=newsize;
}

void Sampler::eResize(size_t newsize) {
  if (newsize>1023) {
    return;
  }
  if (newsize>eSize) {
    //printf("set\n");
    for (size_t i=eSize; i<newsize; i++) {
      //printf("seeet\n");
      memset(&e[i],0,sizeof(envl));
    }
  }
  eSize=newsize;
}

void Sampler::sResize(size_t newsize) {
  while (abusy) {
    printf("audio is busy. waiting.\n");
    SDL_Delay(1);
  }
  busy=true;
  smp* t;
  t=new smp[newsize];
  for (size_t i=0; i<sSize && i<newsize; i++) {
    memcpy(&t[i],&s[i],sizeof(smp));
  }
  sSize=newsize;
  delete[] s;
  s=t;
  for (int i=0; i<vSize; i++) {
    v[i].sample=&s[i];
  }
  busy=false;
}

void Sampler::pResize(envp** which, size_t* cursize, size_t newsize) {
  envp* t;
  t=new envp[newsize];
  for (size_t i=0; (i<(*cursize) && i<newsize); i++) {
    memcpy(&t[i],&which[0][i],sizeof(envp));
  }
  *cursize=newsize;
  delete[] *which;
  *which=t;
}
