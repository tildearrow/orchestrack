#include "sampler.h"
#include <SDL2/SDL.h>

Sampler ins;

unsigned char** ev;

double pc;
int pc1, pc2;

float* pt;

int main() {
  float* pt;
  SDL_Init(SDL_INIT_TIMER);
  TTF_Init();
  printf("initializing sampler...\n");
  if (!ins.init(0,2)) {
    printf("no, sorry.\n"); return 1;
  }
  printf("initializing audio...\n");
  printf("benchmark start!\n");
  pt=ins.getSample();
  ev=new unsigned char*[128];
  for (int i=0; i<128; i++) {
    ev[i]=new unsigned char[4];
    ev[i][0]=0x90;
    ev[i][1]=i;
    ev[i][2]=0x7f;
    ins.submitEvent(ev[i]);
  }
  for (int i=0; i<88200; i++) {
    pc1=SDL_GetPerformanceCounter();
    pt=ins.getSample();
    pc2=SDL_GetPerformanceCounter();
    pc+=(double)(pc2-pc1)/(double)SDL_GetPerformanceFrequency();
  }
  printf("total: %f seconds for 88200 samples (%fx realtime)\n",pc,2.0/pc);
  return 0;
}
