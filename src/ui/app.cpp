#include "app.h"

int OTrackApp::init() {
  quit=false;
  
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  
  dw=1280; dh=800;
  w=SDL_CreateWindow("orchestrack",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,dw,dh,SDL_WINDOW_RESIZABLE);
  if (w==NULL) {
    fprintf(stderr,"error: unable to create window: %s\n",SDL_GetError());
    return 1;
  }
  
  r=SDL_CreateRenderer(w,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_TARGETTEXTURE);
  if (r==NULL) {
    fprintf(stderr,"error: unable to create renderer: %s\n",SDL_GetError());
    return 1;
  }
  
  // load font
  f=new font;
  f->setrenderer(r);
  #ifdef __ANDROID__
  f->load("/system/fonts/Roboto-Regular.ttf",16);
  #elif __linux__
  f->load("/usr/share/fonts/TTF/Ubuntu-R.ttf",16);
  #elif __APPLE__
  f->load("/System/Library/Fonts/Helvetica.dfont",16);
  #elif _WIN32
  f->load("C:\\Windows\\Fonts\\segoeui.ttf",16);
  #endif
  
  // multi-instrument test!
  
  p=new OTrackEngine;
  p->init();
  
  p->p.ins.resize(1);
  
  p->p.ins[0].i=new Sampler;
  if (!p->p.ins[0].i->init(0,2)) {
    printf("no, sorry.\n"); return 1;
  }
  p->p.ins[0].ui=SDL_CreateTexture(r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_TARGET,740,512);
  p->p.ins[0].bound.x=30;
  p->p.ins[0].bound.y=40;
  p->p.ins[0].bound.w=740;
  p->p.ins[0].bound.h=512;
  SDL_SetTextureBlendMode(testt,SDL_BLENDMODE_BLEND);
  p->p.ins[0].i->setRenderer(r);
  
  return 0;
};

void OTrackApp::drawTopBar() {
  tempc.r=255; tempc.g=255; tempc.b=255; tempc.a=255;
  // menu bar //
  // 2x2 margin.
  // first element is 32 pixels wide (for now)
  // and top bar has a height of 20. with margin, 22.
  // this currently looks ugly, but it'll be improved soon.
  f->draw(16+2,2,tempc,1,0,0,"File");
  SDL_SetRenderDrawColor(r,255,255,255,255);
  SDL_RenderDrawLine(r,0,22,dw,22);
  // main toolbar //
  // toolbar is 40 pixels tall.
  SDL_RenderDrawLine(r,0,62,dw,62);
}

void OTrackApp::drawUI() {
  drawTopBar();
  
  for (int i=0; i<p->p.ins.size(); i++) {
    SDL_SetRenderTarget(r,p->p.ins[0].ui);
    SDL_SetRenderDrawColor(r,0,0,0,255);
    SDL_RenderClear(r);
    p->p.ins[i].i->drawUI();
  }
    
  SDL_SetRenderTarget(r,NULL);
  for (int i=0; i<p->p.ins.size(); i++) {
    SDL_SetRenderDrawColor(r,255,255,255,255);
    bound=p->p.ins[0].bound;
    bound.x--; bound.y--; bound.w+=2; bound.h+=2;
    SDL_RenderDrawRect(r,&bound);
    if (selWindow==i) {
      SDL_SetRenderDrawColor(r,255,255,255,255);
    } else {
      SDL_SetRenderDrawColor(r,128,128,128,255);
    }
    bound.y-=24; bound.h=25;
    SDL_RenderDrawRect(r,&bound);
    SDL_RenderCopy(r,p->p.ins[0].ui,NULL,&p->p.ins[0].bound);
  }
    
  SDL_RenderDrawPoint(r,count,20);
  count++;
}

void OTrackApp::mouseUp(int button) {
  if (windowDrag) {
    windowDrag=false;
  }
  for (int i=0; i<p->p.ins.size(); i++) {
    p->p.ins[i].i->mouseEvent(1,button,mouse.x-p->p.ins[0].bound.x,mouse.y-p->p.ins[0].bound.y,0);
  }
}

void OTrackApp::mouseDown(int button) {
  if (selWindow!=-1) {
    windowDrag=true;
    wDx=mouse.x-p->p.ins[selWindow].bound.x;
    wDy=mouse.y-p->p.ins[selWindow].bound.y;
  }
  for (int i=0; i<p->p.ins.size(); i++) {
    p->p.ins[i].i->mouseEvent(2,button,mouse.x-p->p.ins[0].bound.x,mouse.y-p->p.ins[0].bound.y,0);
  }
}

void OTrackApp::mouseMove() {
  if (!windowDrag) {
    selWindow=-1;
  }
  for (int i=0; i<p->p.ins.size(); i++) {
    if (!windowDrag) {
      if (PointInRect(mouse.x,mouse.y,p->p.ins[0].bound.x,p->p.ins[0].bound.y-24,p->p.ins[0].bound.x+p->p.ins[0].bound.w,p->p.ins[0].bound.y)) {
        selWindow=i;
      }
    } else {
      p->p.ins[selWindow].bound.x=mouse.x-wDx;
      p->p.ins[selWindow].bound.y=mouse.y-wDy;
    }
    p->p.ins[i].i->mouseEvent(0,0,mouse.x-p->p.ins[0].bound.x,mouse.y-p->p.ins[0].bound.y,0);
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
          p->p.ins[0].i->mouseEvent(3,0,e.wheel.x,e.wheel.y,0);
          break;
        case SDL_WINDOWEVENT:
          switch (e.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
              dw=e.window.data1;
              dh=e.window.data2;
              break;
          }
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
