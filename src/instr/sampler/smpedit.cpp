#include "sampler.h"

void Sampler::seMouseMove(int button) {
  if (showLoad || showSampleSel || showEnvSel) {
    return;
  }
  hover(690,10,690+40,10+20,&sloadS);
  hover(630,40,630+40,40+20,&seupS);
  hover(580,40,580+40,40+20,&sedownS);
  
  hover(10,10,10+60,10+20,&sselectS);
  
  hover(10,70,10+70,70+20,&sevolS);
  hover(375,70,375+70,70+20,&sepanS);
  hover(10,200,10+70,200+20,&sepitchS);
  hover(375,200,375+70,200+20,&secutS);
  hover(10,330,10+70,330+20,&seresS);
  
  kVolAmp->mouseMove(mouse.x,mouse.y);
  kVolCap->mouseMove(mouse.x,mouse.y);
  kPanAmp->mouseMove(mouse.x,mouse.y);
  kPanCap->mouseMove(mouse.x,mouse.y);
  kPitchAmp->mouseMove(mouse.x,mouse.y);
  kPitchCap->mouseMove(mouse.x,mouse.y);
  kCutAmp->mouseMove(mouse.x,mouse.y);
  kCutCap->mouseMove(mouse.x,mouse.y);
  kResAmp->mouseMove(mouse.x,mouse.y);
  kResCap->mouseMove(mouse.x,mouse.y);
}

void Sampler::seMouseDown(int button) {
  if (showLoad || showSampleSel || showEnvSel) {
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
  
  if (PointInRect(mouse.x,mouse.y,10,70,10+70,70+20)) {
    sevolS=2;
  }
  if (PointInRect(mouse.x,mouse.y,375,70,375+70,70+20)) {
    sepanS=2;
  }
  if (PointInRect(mouse.x,mouse.y,10,200,10+70,200+20)) {
    sepitchS=2;
  }
  if (PointInRect(mouse.x,mouse.y,375,200,375+70,200+20)) {
    secutS=2;
  }
  if (PointInRect(mouse.x,mouse.y,10,330,10+70,330+20)) {
    seresS=2;
  }
  
  kVolAmp->mouseDown(mouse.x,mouse.y,button);
  kVolCap->mouseDown(mouse.x,mouse.y,button);
  kPanAmp->mouseDown(mouse.x,mouse.y,button);
  kPanCap->mouseDown(mouse.x,mouse.y,button);
  kPitchAmp->mouseDown(mouse.x,mouse.y,button);
  kPitchCap->mouseDown(mouse.x,mouse.y,button);
  kCutAmp->mouseDown(mouse.x,mouse.y,button);
  kCutCap->mouseDown(mouse.x,mouse.y,button);
  kResAmp->mouseDown(mouse.x,mouse.y,button);
  kResCap->mouseDown(mouse.x,mouse.y,button);
}

void Sampler::seMouseUp(int button) {
  if (showLoad || showSampleSel || showEnvSel) {
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
  
  if (sevolS!=1) {
    sevolS=PointInRect(mouse.x,mouse.y,10,70,10+70,70+20);
    if (sevolS) {
      prepareEnvSel(true);
      showEnvSel=true;
      envSTarget=1;
      loadHIndex=s[curSample].envVol+1;
    }
  }
  if (sepanS!=1) {
    sepanS=PointInRect(mouse.x,mouse.y,375,70,375+70,70+20);
    if (sepanS) {
      prepareEnvSel(true);
      showEnvSel=true;
      envSTarget=2;
      loadHIndex=s[curSample].envPan+1;
    }
  }
  if (sepitchS!=1) {
    sepitchS=PointInRect(mouse.x,mouse.y,10,200,10+70,200+20);
    if (sepitchS) {
      prepareEnvSel(true);
      showEnvSel=true;
      envSTarget=3;
      loadHIndex=s[curSample].envPitch+1;
    }
  }
  if (secutS!=1) {
    secutS=PointInRect(mouse.x,mouse.y,375,200,375+70,200+20);
    if (secutS) {
      prepareEnvSel(true);
      showEnvSel=true;
      envSTarget=4;
      loadHIndex=s[curSample].envCut+1;
    }
  }
  if (seresS!=1) {
    seresS=PointInRect(mouse.x,mouse.y,10,330,10+70,330+20);
    if (seresS) {
      prepareEnvSel(true);
      showEnvSel=true;
      envSTarget=5;
      loadHIndex=s[curSample].envRes+1;
    }
  }
  
  kVolAmp->mouseUp(mouse.x,mouse.y,button);
  kVolCap->mouseUp(mouse.x,mouse.y,button);
  kPanAmp->mouseUp(mouse.x,mouse.y,button);
  kPanCap->mouseUp(mouse.x,mouse.y,button);
  kPitchAmp->mouseUp(mouse.x,mouse.y,button);
  kPitchCap->mouseUp(mouse.x,mouse.y,button);
  kCutAmp->mouseUp(mouse.x,mouse.y,button);
  kCutCap->mouseUp(mouse.x,mouse.y,button);
  kResAmp->mouseUp(mouse.x,mouse.y,button);
  kResCap->mouseUp(mouse.x,mouse.y,button);
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
  tempr.w=275; tempr1.w=275;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,spathshort,&tempr1,&tempr);
  if (s[curSample].envVol==-1) {
    f->draw((180+275)/2,70,tempc,1,0,0,"<-off->");
  } else {
    f->draw(93,70,tempc,0,0,0,e[s[curSample].envVol].name[0]);
  }
  
  tempr.x=10; tempr1.x=70*sevolS;
  tempr.y=70; tempr1.y=0;
  tempr.w=70;  tempr1.w=70;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,senvsel,&tempr1,&tempr);
  f->draw(45,70,tempc,1,0,0,"Volume");
  
  kVolAmp->setPos(10,100);
  kVolAmp->draw();
  f->draw(42,170,tempc,1,0,0,"Amount");
  
  kVolCap->setPos(84,100);
  kVolCap->draw();
  f->draw(116,170,tempc,1,0,0,"Handicap");
  
  // panning //
  tempr.x=455;  tempr1.x=0;
  tempr.y=70; tempr1.y=0;
  tempr.w=275; tempr1.w=275;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,spathshort,&tempr1,&tempr);
  if (s[curSample].envPan==-1) {
    f->draw((455+455+275)/2,70,tempc,1,0,0,"<-off->");
  } else {
    f->draw(458,70,tempc,0,0,0,e[s[curSample].envPan].name[0]);
  }
  
  tempr.x=375; tempr1.x=70*sepanS;
  tempr.y=70; tempr1.y=0;
  tempr.w=70;  tempr1.w=70;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,senvsel,&tempr1,&tempr);
  f->draw(410,70,tempc,1,0,0,"Panning");
  
  kPanAmp->setPos(375,100);
  kPanAmp->draw();
  f->draw(407,170,tempc,1,0,0,"Amount");
  
  kPanCap->setPos(449,100);
  kPanCap->draw();
  f->draw(481,170,tempc,1,0,0,"Handicap");
  
  // pitch //
  tempr.x=90;  tempr1.x=0;
  tempr.y=200; tempr1.y=0;
  tempr.w=275; tempr1.w=275;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,spathshort,&tempr1,&tempr);
  if (s[curSample].envPitch==-1) {
    f->draw((180+275)/2,200,tempc,1,0,0,"<-off->");
  } else {
    f->draw(93,200,tempc,0,0,0,e[s[curSample].envPitch].name[0]);
  }
  
  tempr.x=10; tempr1.x=70*sepitchS;
  tempr.y=200; tempr1.y=0;
  tempr.w=70;  tempr1.w=70;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,senvsel,&tempr1,&tempr);
  f->draw(45,200,tempc,1,0,0,"Pitch");
  
  kPitchAmp->setPos(10,230);
  kPitchAmp->draw();
  f->draw(42,300,tempc,1,0,0,"Amount");
  
  kPitchCap->setPos(84,230);
  kPitchCap->draw();
  f->draw(116,300,tempc,1,0,0,"Handicap");
  
  // cutoff //
  tempr.x=455;  tempr1.x=0;
  tempr.y=200; tempr1.y=0;
  tempr.w=275; tempr1.w=275;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,spathshort,&tempr1,&tempr);
  if (s[curSample].envCut==-1) {
    f->draw((455+455+275)/2,200,tempc,1,0,0,"<-off->");
  } else {
    f->draw(458,200,tempc,0,0,0,e[s[curSample].envCut].name[0]);
  }
  
  tempr.x=375; tempr1.x=70*secutS;
  tempr.y=200; tempr1.y=0;
  tempr.w=70;  tempr1.w=70;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,senvsel,&tempr1,&tempr);
  f->draw(410,200,tempc,1,0,0,"Cutoff");
  
  kCutAmp->setPos(375,230);
  kCutAmp->draw();
  f->draw(407,300,tempc,1,0,0,"Amount");
  
  kCutCap->setPos(449,230);
  kCutCap->draw();
  f->draw(481,300,tempc,1,0,0,"Handicap");
  
  // resonance //
  tempr.x=90;  tempr1.x=0;
  tempr.y=330; tempr1.y=0;
  tempr.w=275; tempr1.w=275;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,spathshort,&tempr1,&tempr);
  if (s[curSample].envRes==-1) {
    f->draw((180+275)/2,330,tempc,1,0,0,"<-off->");
  } else {
    f->draw(93,330,tempc,0,0,0,e[s[curSample].envRes].name[0]);
  }
  
  tempr.x=10; tempr1.x=70*seresS;
  tempr.y=330; tempr1.y=0;
  tempr.w=70;  tempr1.w=70;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,senvsel,&tempr1,&tempr);
  f->draw(45,330,tempc,1,0,0,"Resonan.");
  
  kResAmp->setPos(10,360);
  kResAmp->draw();
  f->draw(42,430,tempc,1,0,0,"Amount");
  
  kResCap->setPos(84,360);
  kResCap->draw();
  f->draw(116,430,tempc,1,0,0,"Handicap");
  
  upDown();
}
