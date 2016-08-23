#include "sampler.h"
#include <SDL2/SDL.h>
#include <unistd.h>

Sampler ins;
SDL_AudioSpec* as;
SDL_AudioSpec* ras;
SDL_AudioDeviceID ai;

SDL_Window* win;
SDL_Renderer* r;

SDL_Event* e;
bool q;

static void audio(void* userdata, unsigned char* stream, int len) {
  float* s=(float*)stream;
  for (int i=0; i<len/8; i++) {
    float* pt;
    pt=ins.getSample();
    s[i*2]=pt[0];
    s[(i*2)+1]=pt[1];
  }
}

int main() {
  q=false;
  e=new SDL_Event;
  SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO);
  as=new SDL_AudioSpec;
  ras=new SDL_AudioSpec;
  as->freq=44100; as->format=AUDIO_F32; as->channels=2; as->samples=1024; as->callback=audio; as->userdata=NULL;
  printf("initializing sampler...\n");
  if (!ins.init(0,2)) {
    printf("no, sorry.\n"); return 1;
  }
  printf("initializing audio...\n");
  ai=SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0,0),0,as,ras,SDL_AUDIO_ALLOW_ANY_CHANGE);
  printf("initializing UI...\n");
  win=SDL_CreateWindow("Sampler",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,640,400,0);
  r=SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
  ins.setRenderer(r);

  printf("success.\n");
  SDL_PauseAudioDevice(ai,0);
  while (1) {
    while (SDL_PollEvent(e)) {
      switch (e->type) {
        case SDL_QUIT:
          q=true;
      }
    }
    SDL_RenderClear(r);
    ins.drawUI();
    SDL_RenderPresent(r);
    if (q) {break;}
  }
  return 0;
}
