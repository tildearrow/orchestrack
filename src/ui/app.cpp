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
  
  p=new OTrackProject;
  
  p->ins.resize(1);
  
  p->ins[0].i=new Sampler;
  if (!p->ins[0].i->init(0,2)) {
    printf("no, sorry.\n"); return 1;
  }
  p->ins[0].ui=SDL_CreateTexture(r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_TARGET,740,512);
  p->ins[0].bound.x=30;
  p->ins[0].bound.y=40;
  p->ins[0].bound.w=740;
  p->ins[0].bound.h=512;
  SDL_SetTextureBlendMode(testt,SDL_BLENDMODE_BLEND);
  p->ins[0].i->setRenderer(r);
  
  return 0;
};

void OTrackApp::drawUI() {
  for (int i=0; i<p->ins.size(); i++) {
    SDL_SetRenderTarget(r,p->ins[0].ui);
    SDL_RenderClear(r);
    p->ins[i].i->drawUI();
  }
    
  SDL_SetRenderTarget(r,NULL);
  for (int i=0; i<p->ins.size(); i++) {
    SDL_SetRenderDrawColor(r,255,255,255,255);
    bound=p->ins[0].bound;
    bound.x--; bound.y--; bound.w+=2; bound.h+=2;
    SDL_RenderDrawRect(r,&bound);
    if (selWindow==i) {
      SDL_SetRenderDrawColor(r,255,255,255,255);
    } else {
      SDL_SetRenderDrawColor(r,128,128,128,255);
    }
    bound.y-=24; bound.h=25;
    SDL_RenderDrawRect(r,&bound);
    SDL_RenderCopy(r,p->ins[0].ui,NULL,&p->ins[0].bound);
  }
    
  SDL_RenderDrawPoint(r,count,20);
  count++;
}

void OTrackApp::mouseUp(int button) {
  for (int i=0; i<p->ins.size(); i++) {
    p->ins[i].i->mouseEvent(1,button,mouse.x-p->ins[0].bound.x,mouse.y-p->ins[0].bound.y,0);
  }
}

void OTrackApp::mouseDown(int button) {
  for (int i=0; i<p->ins.size(); i++) {
    p->ins[i].i->mouseEvent(2,button,mouse.x-p->ins[0].bound.x,mouse.y-p->ins[0].bound.y,0);
  }
}

void OTrackApp::mouseMove() {
  selWindow=-1;
  for (int i=0; i<p->ins.size(); i++) {
    if (PointInRect(mouse.x,mouse.y,p->ins[0].bound.x,p->ins[0].bound.y-24,p->ins[0].bound.x+p->ins[0].bound.w,p->ins[0].bound.y)) {
      selWindow=i;
    }
    p->ins[i].i->mouseEvent(0,0,mouse.x-p->ins[0].bound.x,mouse.y-p->ins[0].bound.y,0);
  }
}

int OTrackApp::loop() {
  while (1) {
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
        case SDL_QUIT:
          quit=true;
        case SDL_MOUSEBUTTONUP:
          mouse.x=e.button.x;
          mouse.y=e.button.y;
          mouse.b[e.button.button-1]=0;
          mouseUp(e.button.button-1);
          break;
        case SDL_MOUSEBUTTONDOWN:
          mouse.x=e.button.x;
          mouse.y=e.button.y;
          mouse.b[e.button.button-1]=1;
          mouseDown(e.button.button-1);
          break;
        case SDL_MOUSEMOTION:
          mouse.x=e.button.x;
          mouse.y=e.button.y;
          mouseMove();
          break;
        case SDL_MOUSEWHEEL:
          p->ins[0].i->mouseEvent(3,0,e.wheel.x,e.wheel.y,0);
          break;
      }
    }
    SDL_SetRenderDrawColor(r,0,0,0,255);
    SDL_RenderClear(r);
    drawUI();
    SDL_RenderPresent(r);
    if (quit) {
      break;
    }
  }
  return 0;
}
