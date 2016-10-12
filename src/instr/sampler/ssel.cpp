#include "sampler.h"

void Sampler::prepareSampleSel() {
  clearList();
  for (size_t i=0; i<sSize; i++) {
    feedList(s[i].path[0],"",255,255,255,255);
  }
}

void Sampler::drawSampleSel() {
  tempr.x=0;
  tempr.y=0;
  tempr.w=740;
  tempr.h=512;
  SDL_SetRenderDrawColor(r,0,0,0,192);
  SDL_RenderFillRect(r,&tempr);
  
  tempr.x=20; tempr1.x=0;
  tempr.y=20; tempr1.y=0;
  tempr.w=700;  tempr1.w=700;
  tempr.h=472;  tempr1.h=472;
  SDL_RenderCopy(r,sloadform,&tempr1,&tempr);
  
  tempr.x=80; tempr1.x=0;
  tempr.y=30; tempr1.y=0;
  tempr.w=580;  tempr1.w=580;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,slfdir,&tempr1,&tempr);
  
  tempr.x=670; tempr1.x=0;
  tempr.y=30; tempr1.y=0;
  tempr.w=40;  tempr1.w=40;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,sload,&tempr1,&tempr);
  
  tempr.x=30; tempr1.x=40*supS;
  SDL_RenderCopy(r,sload,&tempr1,&tempr);
  
  tempr.y=462; tempr1.y=0;
  
  tempr.x=610; tempr1.x=0;
  SDL_RenderCopy(r,sload,&tempr1,&tempr);
  
  tempr.x=660; tempr1.x=50*scancelS;
  tempr.w=50;  tempr1.w=50;
  SDL_RenderCopy(r,scancel,&tempr1,&tempr);
  
  tempr.x=30; tempr1.x=0;
  tempr.y=462; tempr1.y=0;
  tempr.w=570;  tempr1.w=570;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,slfpath,&tempr1,&tempr);
  
  f->draw(50,30,tempc,1,0,0,"New");
  f->draw(690,30,tempc,1,0,0,"Del");
  
  f->draw(630,462,tempc,1,0,0,"Select");
  f->draw(685,462,tempc,1,0,0,"Cancel");
  
  f->draw(370,30,tempc,1,0,0,"Select Sample");
  f->draw(33,462,tempc,0,0,0,sfname);
  drawList();
}
