#include "sampler.h"

void Sampler::seMouseMove(int button) {
  if (showLoad || showSampleSel) {
    return;
  }
  hover(690,10,690+40,10+20,&sloadS);
  hover(630,40,630+40,40+20,&seupS);
  hover(580,40,580+40,40+20,&sedownS);
  
  hover(10,10,10+60,10+20,&sselectS);
}

void Sampler::seMouseDown(int button) {
  if (showLoad || showSampleSel) {
    return;
  }
  if (PointInRect(mouse.x,mouse.y,690,10,690+40,10+20)) {
    sloadS=2;
  }
  if (PointInRect(mouse.x,mouse.y,10,10,10+60,10+20)) {
    sselectS=2;
  }
  // rate
  if (PointInRect(mouse.x,mouse.y,630,40,630+40,40+20)) {
    seupS=2;
    if (button!=1) {
      doUp=true;
      doXTarget=0;
    } else {
      s[curSample].rate*=2;
    }
  }
  if (PointInRect(mouse.x,mouse.y,580,40,580+40,40+20)) {
    sedownS=2;
    if (button!=1) {
      doDown=true;
      doXTarget=0;
    } else {
      s[curSample].rate/=2;
    }
  }
}

void Sampler::seMouseUp(int button) {
  if (showLoad || showSampleSel) {
    return;
  }
  if (sloadS!=1) {
    sloadS=PointInRect(mouse.x,mouse.y,690,10,690+40,10+20);
    if (sloadS) {
      printf("load?\n");
      readDir(wd.c_str());
      showLoad=true;
      loadHIndex=-1;
    }
  }
  if (sselectS!=1) {
    sselectS=PointInRect(mouse.x,mouse.y,10,10,10+60,10+20);
    if (sselectS) {
      printf("select?\n");
      prepareSampleSel();
      showSampleSel=true;
      loadHIndex=-1;
    }
  }
  if (seupS!=1) {
    seupS=PointInRect(mouse.x,mouse.y,630,40,630+40,40+20);
    doUp=false;
  }
  if (sedownS!=1) {
    sedownS=PointInRect(mouse.x,mouse.y,580,40,580+40,40+20);
    doDown=false;
  }
}

void Sampler::drawSampleEdit() {
  f->draw(10,40,tempc,0,0,0,"Rate");
  tempr.x=80;  tempr1.x=0;
  tempr.y=10; tempr1.y=0;
  tempr.w=600; tempr1.w=600;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,spath,&tempr1,&tempr);
  tempr.x=690; tempr1.x=40*sloadS;
  tempr.y=10; tempr1.y=0;
  tempr.w=40;  tempr1.w=40;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,sload,&tempr1,&tempr);
  tempr.x=630; tempr1.x=40*seupS;
  tempr.y=40; tempr1.y=0;
  SDL_RenderCopy(r,sload,&tempr1,&tempr);
  tempr.x=580; tempr1.x=40*sedownS;
  SDL_RenderCopy(r,sload,&tempr1,&tempr);
  tempr.x=680; tempr1.x=50;
  tempr.y=40; tempr1.y=0;
  tempr.w=50;  tempr1.w=50;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,scancel,&tempr1,&tempr);
  
  tempr.x=10; tempr1.x=60*sselectS;
  tempr.y=10; tempr1.y=0;
  tempr.w=60;  tempr1.w=60;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,sselect,&tempr1,&tempr);
  f->draw(40,10,tempc,1,0,0,"Sample");
  
  f->draw(83,10,tempc,0,0,0,s[curSample].path[0]);
  f->drawf(83,40,tempc,0,0,"%f",s[curSample].rate);
  f->draw(710,10,tempc,1,0,0,"Load");
  f->draw(705,40,tempc,1,0,0,"Keypad");
  f->draw(650,40,tempc,1,0,0,"Up");
  f->draw(600,40,tempc,1,0,0,"Down");
  
  /// envelopes ///
  
  // volume //
  tempr.x=90;  tempr1.x=0;
  tempr.y=70; tempr1.y=0;
  tempr.w=640; tempr1.w=640;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,spathlarge,&tempr1,&tempr);
  
  tempr.x=10; tempr1.x=60*sselectS;
  tempr.y=70; tempr1.y=0;
  tempr.w=70;  tempr1.w=70;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,senvsel,&tempr1,&tempr);
  f->draw(45,70,tempc,1,0,0,"Volume");
  
  // panning //
  tempr.x=90;  tempr1.x=0;
  tempr.y=100; tempr1.y=0;
  tempr.w=640; tempr1.w=640;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,spathlarge,&tempr1,&tempr);
  
  tempr.x=10; tempr1.x=60*sselectS;
  tempr.y=100; tempr1.y=0;
  tempr.w=70;  tempr1.w=70;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,senvsel,&tempr1,&tempr);
  f->draw(45,100,tempc,1,0,0,"Panning");
  
  // pitch //
  tempr.x=90;  tempr1.x=0;
  tempr.y=130; tempr1.y=0;
  tempr.w=640; tempr1.w=640;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,spathlarge,&tempr1,&tempr);
  
  tempr.x=10; tempr1.x=60*sselectS;
  tempr.y=130; tempr1.y=0;
  tempr.w=70;  tempr1.w=70;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,senvsel,&tempr1,&tempr);
  f->draw(45,130,tempc,1,0,0,"Pitch");
  
  // cutoff //
  tempr.x=90;  tempr1.x=0;
  tempr.y=160; tempr1.y=0;
  tempr.w=640; tempr1.w=640;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,spathlarge,&tempr1,&tempr);
  
  tempr.x=10; tempr1.x=60*sselectS;
  tempr.y=160; tempr1.y=0;
  tempr.w=70;  tempr1.w=70;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,senvsel,&tempr1,&tempr);
  f->draw(45,160,tempc,1,0,0,"Cutoff");
  
  // resonance //
  tempr.x=90;  tempr1.x=0;
  tempr.y=190; tempr1.y=0;
  tempr.w=640; tempr1.w=640;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,spathlarge,&tempr1,&tempr);
  
  tempr.x=10; tempr1.x=60*sselectS;
  tempr.y=190; tempr1.y=0;
  tempr.w=70;  tempr1.w=70;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,senvsel,&tempr1,&tempr);
  f->draw(45,190,tempc,1,0,0,"Resonan.");
  
  upDown();
}
