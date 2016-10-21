#include "sampler.h"

void Sampler::prepareEnvSel(bool addNone) {
  clearList();
  if (addNone) {
    feedList("<-off->","",192,192,192,255);
  }
  for (size_t i=0; i<eSize; i++) {
    feedList(e[i].name[0],"",255,255,255,255);
  }
}

void Sampler::initEnv(int which) {
  char* sl;
  printf("which: %d\n",which);
  e[which].name=new string;
  e[which].name[0]="Envelope";
  sl=new char[21];
  sprintf(sl,"%d",which+1);
  e[which].name[0]+=sl;
  delete[] sl;
  e[which].susStart=-1;
  e[which].susEnd=-1;
  e[which].loopStart=-1;
  e[which].loopEnd=-1;
  e[which].p=new envp[3];
  e[which].pSize=3;
  e[which].relMode=1;
  e[which].p[0].type=0;
  e[which].p[1].type=0;
  e[which].p[2].type=0;
  e[which].p[0].value=1;
  e[which].p[1].value=0.3;
  e[which].p[2].value=0;
  e[which].p[0].time=0;
  e[which].p[1].time=5000;
  e[which].p[2].time=150000;
}

void Sampler::drawEnvSel() {
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
  
  f->draw(370,30,tempc,1,0,0,"Select Envelope");
  f->draw(33,462,tempc,0,0,0,sfname);
  drawList();
}
