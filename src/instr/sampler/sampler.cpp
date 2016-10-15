// TODO: envelopes!
#include "sampler.h"

OTrackInsSpec* Sampler::getspec() {
  OTrackInsSpec* aspec;
  aspec=new OTrackInsSpec;
  aspec->name="Sampler";
  aspec->desc="default sampler";
  aspec->author="tildearrow";
  aspec->copyright="none";
  aspec->license="none";
  return aspec;
}

void Sampler::reset() {
  vResize(0);
}

void Sampler::setRenderer(SDL_Renderer* renderer) {
  r=renderer;
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
  // init UI
  tempc.r=0;
  tempc.g=128;
  tempc.b=255;
  tempc.a=255;
  but=drawButton(r,0,0,128,64,tempc,16);
  tempc.r=34;
  tempc.g=68;
  tempc.b=98;
  tempc.a=255;
  grid=drawButton(r,0,0,720,340,tempc,4);
  tempc.r=64;
  tempc.g=56;
  tempc.b=6;
  tempc.a=255;
  envbg=drawButton(r,0,0,720,300,tempc,4);
  tempc.r=16;
  tempc.g=16;
  tempc.b=16;
  tempc.a=255;
  spath=drawButton(r,0,0,600,20,tempc,4);
  spathlarge=drawButton(r,0,0,640,20,tempc,4);
  slfdir=drawButton(r,0,0,580,20,tempc,4);
  slfpath=drawButton(r,0,0,570,20,tempc,4);
  srange=drawButton(r,0,0,60,36,tempc,4);
  tempc.r=64;
  tempc.g=64;
  tempc.b=64;
  tempc.a=255;
  sload=drawButton(r,0,0,40,20,tempc,4);
  scancel=drawButton(r,0,0,50,20,tempc,4);
  sselect=drawButton(r,0,0,60,20,tempc,4);
  senvsel=drawButton(r,0,0,70,20,tempc,4);
  srangebutton=drawButton(r,0,0,40,24,tempc,4);
  tempc.r=32;
  tempc.g=32;
  tempc.b=32;
  tempc.a=255;
  sloadform=drawButton(r,0,0,700,472,tempc,10);
  tempc.r=48;
  tempc.g=48;
  tempc.b=48;
  tempc.a=255;
  slflist=drawButton(r,0,0,680,392,tempc,2);
  tempc.r=48;
  tempc.g=48;
  tempc.b=48;
  tempc.a=255;
  smode=drawButton(r,0,0,175,20,tempc,2);
  tempc.r=128;
  tempc.g=128;
  tempc.b=128;
  tempc.a=255;
  smodeactive=drawButton(r,0,0,175,20,tempc,4);
  // knobs //
  // test knob
  testk=new OTrackKnob(r,32,0,255,0);
  testk->setOut(&s[0].volAmt);
  testk->setRange(0.0,1.0,1.0);
  // volume knobs
  kVolAmp=new OTrackKnob(r,32,0,255,0);
  kVolAmp->setOut(&s[0].volAmt);
  kVolAmp->setRange(0.0,1.0,1.0);
  kVolCap=new OTrackKnob(r,32,0,255,0);
  kVolCap->setOut(&s[0].volCap);
  kVolCap->setRange(-1.0,1.0,0.0);
  // panning knobs
  kPanAmp=new OTrackKnob(r,32,255,255,0);
  kPanAmp->setOut(&s[0].panAmt);
  kPanAmp->setRange(-1.0,1.0,0.0);
  kPanCap=new OTrackKnob(r,32,255,255,0);
  kPanCap->setOut(&s[0].panCap);
  kPanCap->setRange(-1.0,1.0,0.0);
  
  showLoad=false;
  // init eye candy //
  aBBPos=-30;
  fc=0;
}

void Sampler::drawSummary() {
  
}

void Sampler::drawUI() {
  tempc.r=255;
  tempc.g=255;
  tempc.b=255;
  tempc.a=255;
  
  SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
  
  aBBPos*=0.7f;
  
  if (aBBPos>-0.02 && aBBPos!=0) {
    aBBPos=0;
  }
  
  tempr.x=8;  tempr1.x=0;
  tempr.y=482-(int)aBBPos; tempr1.y=0;
  tempr.w=175; tempr1.w=175;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,(curView==0)?(smodeactive):(smode),&tempr1,&tempr);
  
  tempr.x=191;  tempr1.x=0;
  SDL_RenderCopy(r,(curView==1)?(smodeactive):(smode),&tempr1,&tempr);
  
  tempr.x=374;  tempr1.x=0;
  SDL_RenderCopy(r,(curView==2)?(smodeactive):(smode),&tempr1,&tempr);
  
  tempr.x=557;  tempr1.x=0;
  SDL_RenderCopy(r,(curView==3)?(smodeactive):(smode),&tempr1,&tempr);
  
  f->draw(95,482-(int)aBBPos,tempc,1,0,0,"Summary");
  f->draw(278,482-(int)aBBPos,tempc,1,0,0,"Grid");
  f->draw(462,482-(int)aBBPos,tempc,1,0,0,"Editor");
  f->draw(644,482-(int)aBBPos,tempc,1,0,0,"Envelope");
  
  switch (curView) {
    case 0: drawSummary(); break;
    case 1: drawGrid(); break;
    case 2: drawSampleEdit(); break;
    case 3: drawEnvEdit(); break;
  }
  
  if (showLoad) {
    drawLoadUI();
  }
  
  if (showSampleSel) {
    drawSampleSel();
  }
  
  /***
  SDL_SetRenderDrawColor(r,(mouse.b[0])?(0):(255),(mouse.b[1])?(0):(255),(mouse.b[2])?(0):(255),255);
  SDL_RenderDrawLine(r,mouse.x,mouse.y,0,0);
  ***/
  
  SDL_SetRenderDrawColor(r,255,255,255,255);
  
  SDL_SetRenderDrawColor(r,0,0,0,255);
}

bool Sampler::init(int inChannels, int outChannels) {
  if (inChannels==0) {
    sample=new float[outChannels];
    outCh=outChannels;
    char* twd;
#ifndef _WIN32
    twd=getcwd(NULL,PATH_MAX);
#else
    twd=_getcwd(NULL,MAX_PATH);
#endif
    wd=twd;
    delete[] twd;
    //sResize(1);
    s=new smp[1];
    sSize=1;
    initSample(0);
    /*
    s[0].path="Sample1";
    s[0].noteMin=0;
    s[0].noteMax=127;
    s[0].velMin=0;
    s[0].velMax=127;*/
    /*
    FILE* fo;
    fo=fopen("../share/orchestrack/testsmp.wav","rb");
    lf=readIFF(fo);
    fclose(fo);
    if (lf) {
      lwf=readWAVE(lf);
    }
    sndf=sf_open("../share/orchestrack/testsmp.wav",SFM_READ,&si);*/
    
    e[0].susStart=-1;
    e[0].susEnd=-1;
    e[0].loopStart=-1;
    e[0].loopEnd=-1;
    e[0].p=new envp[3];
    e[0].pSize=3;
    e[0].p[0].type=0;
    e[0].p[1].type=0;
    e[0].p[2].type=0;
    e[0].p[0].value=1;
    e[0].p[1].value=0.3;
    e[0].p[2].value=0;
    e[0].p[0].time=0;
    e[0].p[1].time=5000;
    e[0].p[2].time=150000;
    //tbuf=new float[si.channels];
    /*
    sf_close(sndf);
    delete[] tbuf;*/
    vSize=0;
    //v=new voice[0];
    windowed_fir_init(table);
    showHidden=false;
    selPoint=-1;
    pMenuSel=-1;
    busy=false;
    return true;
  } else {
    return false;
  }
}
