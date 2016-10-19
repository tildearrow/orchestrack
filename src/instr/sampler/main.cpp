#include "sampler.h"
#include <SDL2/SDL.h>
#ifdef HAVE_JACK
#include <jack/jack.h>
#include <jack/midiport.h>
#endif

Sampler ins;

int sr;

SDL_AudioSpec* as;
SDL_AudioSpec* ras;
SDL_AudioDeviceID ai;

#ifdef HAVE_JACK
jack_port_t* ao[2];
jack_port_t* mo;
jack_client_t* ac;
jack_status_t js;
#endif
int ec;
const char* an;

SDL_Window* win;
SDL_Renderer* r;

SDL_Event* e;
bool q;
bool touch;

SDL_DisplayMode dd;
int dw, dh;

#ifdef HAVE_JACK
int audio(jack_nframes_t len, void* arg) {
  float* s[2];
  int nep; // next event position
  int cei; // current event index
  jack_midi_event_t* me;
  void* m;
  m=jack_port_get_buffer(mo,len);
  nep=-1;
  cei=0;
  for (int i=0; i<2; i++) {
    s[i]=(float*)jack_port_get_buffer(ao[i],len);
  }
  ec=jack_midi_get_event_count(m);
  if (ec!=0) {
    me=new jack_midi_event_t[ec];
  }
  for (int i=0; i<ec; i++) {
    jack_midi_event_get(&me[i],m,i);
  }
  if (ec!=0) {
    nep=me[0].time;
  }
  for (int i=0; i<len; i++) {
    float* pt;
    while (nep==i) {
      ins.submitEvent(me[cei].buffer);
      cei++;
      if (cei<ec) {
        nep=me[cei].time;
      } else {
	nep=-1;
      }
    }
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
  mo=jack_port_register(ac,"midi",JACK_DEFAULT_MIDI_TYPE,JackPortIsInput,0);
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
  const char** p;
  p=jack_get_ports(ac,NULL,NULL,JackPortIsInput|JackPortIsPhysical);
  if (p) {
    for (int i=0; i<2; i++) {
      if (jack_connect(ac,jack_port_name(ao[i]),p[i])) {
        fprintf(stderr,"can't connect to system output (i=%d). :(\n",i);
        break;
      }
    }
    delete[] p;
  } else {
    fprintf(stderr,"no physical outputs.\n");
  }
#else
  SDL_PauseAudioDevice(ai,0);
#endif
}

int main() {
  q=false;
  touch=false;
  e=new SDL_Event;
  SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
  TTF_Init();
  printf("initializing sampler...\n");
  if (!ins.init(0,2)) {
    printf("no, sorry.\n"); return 1;
  }
  printf("initializing audio...\n");
  initAudio();
  printf("initializing UI...\n");
  win=SDL_CreateWindow("Sampler",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,740,512,0);
  r=SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
  SDL_RenderClear(r);
  SDL_RenderPresent(r);
  ins.setRenderer(r);
#ifdef __ANDROID__
  SDL_GetCurrentDisplayMode(0,&dd);
  dw=dd.w;
  dh=dd.h;
#else
  dw=740;
  dh=512;
#endif

  printf("success.\n");
  letAudioRun();
  while (1) {
    while (SDL_PollEvent(e)) {
      switch (e->type) {
        case SDL_MOUSEWHEEL:
          printf("wheel %d %d\n",e->wheel.x,e->wheel.y);
          ins.mouseEvent(3,0,e->wheel.x,e->wheel.y,0);
          break;
        case SDL_MOUSEBUTTONUP:
	  if (!touch) {
            ins.mouseEvent(1,e->button.button-1,e->button.x,e->button.y,0);
	  }
          break;
        case SDL_MOUSEBUTTONDOWN:
          if (!touch) {
	     ins.mouseEvent(2,e->button.button-1,e->button.x,e->button.y,0);
	  }
          break;
        case SDL_MOUSEMOTION:
          if (!touch) {
	    ins.mouseEvent(0,0,e->button.x,e->button.y,0);
	  }
          break;
#ifndef __APPLE__
        case SDL_FINGERUP:
	  touch=true;
	  printf("up %d %d %lld\n",(int)(e->tfinger.x*dw),(int)(e->tfinger.y*dh),e->tfinger.fingerId);
          ins.mouseEvent(1,0,(int)(e->tfinger.x*dw),(int)(e->tfinger.y*dh),(int)e->tfinger.fingerId);
          break;
        case SDL_FINGERDOWN:
	  touch=true;
	  printf("down %d %d %lld\n",(int)(e->tfinger.x*dw),(int)(e->tfinger.y*dh),e->tfinger.fingerId);
          ins.mouseEvent(2,0,(int)(e->tfinger.x*dw),(int)(e->tfinger.y*dh),(int)e->tfinger.fingerId);
          break;
        case SDL_FINGERMOTION:
	  touch=true;
          ins.mouseEvent(0,0,(int)(e->tfinger.x*dw),(int)(e->tfinger.y*dh),(int)e->tfinger.fingerId);
          break;
#endif
        case SDL_DROPFILE:
          printf("dropped file!\n");
          FILE* df;
          df=fopen(e->drop.file,"rb");
          if (df) {
            ins.loadState(df);
            fclose(df);
          } else {
            printf("what?\n");
          }
          SDL_free(e->drop.file);
          break;
        case SDL_KEYDOWN:
          ins.keyEvent(1,e->key.timestamp,e->key.repeat,e->key.keysym.scancode,e->key.keysym.sym,e->key.keysym.mod);
          break;
        case SDL_TEXTINPUT:
          printf("input\n");
          ins.textEvent(e->text.text);
          break;
        case SDL_QUIT:
          q=true;
          break;
        default:
          printf("got event %d\n",e->type);
      }
    }
    SDL_RenderClear(r);
    ins.drawUI();
    SDL_RenderPresent(r);
    if (q) {break;}
  }
  return 0;
}

#ifdef __ANDROID__
int SDL_main(int argc, char** argv) {
  main();
}
#endif
