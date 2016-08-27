#include "sampler.h"
#include <SDL2/SDL.h>
#define HAVE_JACK
#include <jack/jack.h>
#include <unistd.h>

Sampler ins;

int sr;

SDL_AudioSpec* as;
SDL_AudioSpec* ras;
SDL_AudioDeviceID ai;

jack_port_t* ao[2];
jack_client_t* ac;
jack_status_t js;
char* an;

SDL_Window* win;
SDL_Renderer* r;

SDL_Event* e;
bool q;

#ifdef HAVE_JACK
int audio(jack_nframes_t len, void* arg) {
  float* s[2];
  for (int i=0; i<2; i++) {
    s[i]=(float*)jack_port_get_buffer(ao[i],len);
  }
  for (int i=0; i<len; i++) {
    float* pt;
    pt=ins.getSample();
    s[0][i]=pt[0];
    s[1][i]=pt[1];
  }
  return 0;
}
#else
static void audio(void* userdata, unsigned char* stream, int len) {
  float* s=(float*)stream;
  for (int i=0; i<len/8; i++) {
    float* pt;
    pt=ins.getSample();
    s[i*2]=pt[0];
    s[(i*2)+1]=pt[1];
  }
}
#endif

int initAudio() {
  printf("initializing audio...\n");
#ifdef HAVE_JACK
  an=new char[8];
  an="sampler";
  ac=jack_client_open(an,JackNoStartServer,&js,NULL);
  if (ac==NULL) {
    printf("failed to init audio!\n");
    return 1;
  }
  jack_set_process_callback(ac,audio,0);
  if (js&JackNameNotUnique) {
    an=jack_get_client_name(ac);
    printf("name not unique. setting to %s",an);
  }
  sr=jack_get_sample_rate(ac);
  ao[0]=jack_port_register(ac,"frontL",JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);
  ao[1]=jack_port_register(ac,"frontR",JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);
  return 0;
#else
  sr=44100;
  as=new SDL_AudioSpec;
  ras=new SDL_AudioSpec;
  as->freq=sr; as->format=AUDIO_F32; as->channels=2; as->samples=1024; as->callback=audio; as->userdata=NULL;
  ai=SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0,0),0,as,ras,SDL_AUDIO_ALLOW_ANY_CHANGE);
  return 0;
#endif
}

void letAudioRun() {
#ifdef HAVE_JACK
  jack_activate(ac);
#else
  SDL_PauseAudioDevice(ai,0);
#endif
}

int main() {
  q=false;
  e=new SDL_Event;
  SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO);
  printf("initializing sampler...\n");
  if (!ins.init(0,2)) {
    printf("no, sorry.\n"); return 1;
  }
  printf("initializing audio...\n");
  initAudio();
  printf("initializing UI...\n");
  win=SDL_CreateWindow("Sampler",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,640,400,0);
  r=SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
  ins.setRenderer(r);

  printf("success.\n");
  letAudioRun();
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
