#include "app.h"

int OTrackApp::init() {
  quit=false;
  
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  
  w=SDL_CreateWindow("orchestrack",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1280,800,0);
  if (w==NULL) {
    fprintf(stderr,"error: unable to create window: %s\n",SDL_GetError());
    return 1;
  }
  
  r=SDL_CreateRenderer(w,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_TARGETTEXTURE);
  if (r==NULL) {
    fprintf(stderr,"error: unable to create renderer: %s\n",SDL_GetError());
    return 1;
  }
  
  // multi-instrument test!
  
  testi=new Sampler;
  if (!testi->init(0,2)) {
    printf("no, sorry.\n"); return 1;
  }
  testt=SDL_CreateTexture(r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_TARGET,740,512);
  bound.x=0; bound.y=20; bound.w=740; bound.h=512;
  SDL_SetTextureBlendMode(testt,SDL_BLENDMODE_BLEND);
  testi->setRenderer(r);
  
  testi1=new Sampler;
  if (!testi1->init(0,2)) {
    printf("no, sorry.\n"); return 1;
  }
  testt1=SDL_CreateTexture(r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_TARGET,740,512);
  bound1.x=500; bound1.y=40; bound1.w=740; bound1.h=512;
  SDL_SetTextureBlendMode(testt1,SDL_BLENDMODE_BLEND);
  testi1->setRenderer(r);
  
  return 0;
};

int OTrackApp::loop() {
  while (1) {
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
        case SDL_QUIT:
          quit=true;
        case SDL_MOUSEBUTTONUP:
          testi->mouseEvent(1,e.button.button-1,e.button.x-bound.x,e.button.y-bound.y,0);
          testi1->mouseEvent(1,e.button.button-1,e.button.x-bound1.x,e.button.y-bound1.y,0);
          break;
        case SDL_MOUSEBUTTONDOWN:
          testi->mouseEvent(2,e.button.button-1,e.button.x-bound.x,e.button.y-bound.y,0);
          testi1->mouseEvent(2,e.button.button-1,e.button.x-bound1.x,e.button.y-bound1.y,0);
          break;
        case SDL_MOUSEMOTION:
          testi->mouseEvent(0,0,e.button.x-bound.x,e.button.y-bound.y,0);
          testi1->mouseEvent(0,0,e.button.x-bound1.x,e.button.y-bound1.y,0);
          break;
        case SDL_MOUSEWHEEL:
          testi->mouseEvent(3,0,e.wheel.x,e.wheel.y,0);
          testi1->mouseEvent(3,0,e.wheel.x,e.wheel.y,0);
          break;
      }
    }
    SDL_SetRenderDrawColor(r,0,0,0,255);
    SDL_RenderClear(r);
    
    SDL_SetRenderTarget(r,testt);
    SDL_RenderClear(r);
    testi->drawUI();
    
    SDL_SetRenderTarget(r,testt1);
    SDL_RenderClear(r);
    testi1->drawUI();
    
    SDL_SetRenderTarget(r,NULL);
    SDL_RenderCopy(r,testt,NULL,&bound);
    SDL_RenderCopy(r,testt1,NULL,&bound1);
    SDL_SetRenderDrawColor(r,255,255,255,255);
    SDL_RenderDrawPoint(r,count,20);
    count++;
    SDL_RenderPresent(r);
    if (quit) {
      break;
    }
  }
  return 0;
}
