#include "application.h"

int OTrackApp::init() {
  quit=false;
  
  SDL_Init(SDL_INIT_VIDEO);
  
  w=SDL_CreateWindow("orchestrack",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1280,800,0);
  if (w==NULL) {
    fprintf(stderr,"error: unable to create window: %s\n",SDL_GetError());
    return 1;
  }
  
  r=SDL_CreateRenderer(w,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
  if (r==NULL) {
    fprintf(stderr,"error: unable to create renderer: %s\n",SDL_GetError());
    return 1;
  }
  
  return 0;
};

int OTrackApp::loop() {
  while (1) {
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
	case SDL_QUIT:
	  quit=true;
      }
    }
    SDL_RenderClear(r);
    SDL_RenderPresent(r);
    if (quit) {
      break;
    }
  }
  return 0;
}