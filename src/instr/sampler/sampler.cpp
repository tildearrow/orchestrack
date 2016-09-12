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

float* Sampler::getSample() {
  float element0, element1, element, definitepos;
  sample[0]=0;
  sample[1]=0;
  ev=(unsigned char*)getEvent();
  while (ev!=NULL) {
    if ((ev[0]>>4)==8) {
      // find voice with properties, then destroy it
      for (int i=0; i<v.size(); i++) {
        if (v[i].chan==(ev[0]&15)) {
          if (v[i].note==ev[1]) {
            printf("erased. %d %.2x %.2x\n",i,v[i].chan,v[i].note);
            v.erase(v.begin()+i); i--;
          }
        }
      }
      /*
      v[ev[0]&15].period=0;
      v[ev[0]&15].f=0;
      v[ev[0]&15].vol=(float)ev[2]/128;*/
    }
    if ((ev[0]>>4)==9) {
      // allocate a voice
      v.resize(v.size()+1);
      int thisv;
      thisv=v.size()-1;
      v[thisv].chan=ev[0]&15;
      v[thisv].note=ev[1];
      printf("allocated. %d %.2x %.2x\n",thisv,v[thisv].chan,v[thisv].note);
      v[thisv].period=0;
      v[thisv].f=pow(2,((float)v[thisv].note-60)/12)*s[0].rate/44100;
      v[thisv].vol=(float)ev[2]/128;
    }
    if ((ev[0]>>4)==0xe) {
      // pitch bend.
      c[ev[0]&15].pitch=(ev[1]+(ev[2]<<7))-0x2000;
      printf("pitch %d\n",c[ev[0]&15].pitch);
      for (int i=0; i<v.size(); i++) {
        if (v[i].chan==(ev[0]&15)) {
          v[i].f=pow(2,((float)v[i].note-60+((float)c[ev[0]&15].pitch/4096.0))/12)*s[0].rate/44100;
        }
      }
    }
    ev=(unsigned char*)getEvent();
  }
  for (int i=0; i<v.size(); i++) {
    definitepos=fmin(v[i].period,s[0].len);
    element0=s[0].data[(int)definitepos];
    element1=s[0].data[(int)definitepos+1];
    element=element0+((element1-element0)*fmod(definitepos,1));
    
    sample[0]+=element*v[i].vol;
    sample[1]+=element*v[i].vol;
    v[i].period+=v[i].f;
  }
  
  for (int i=0; i<v.size(); i++) {
    if (v[i].period>s[0].len) {
      printf("sample finished. %d\n",i);
      v.erase(v.begin()+i); i--;
    }
  }
  sample[0]=sample[0]/4;
  sample[1]=sample[1]/4;
  return sample;
}

void Sampler::mouseEvent(int type, int button, int x, int y, int finger) {
  switch (type) {
    case 0:
      mouse.x=x; mouse.y=y; break;
    case 2:
      mouse.b[button]=1; break;
    case 1:
      mouse.b[button]=0; break;
  }
}

void Sampler::setRenderer(SDL_Renderer* renderer) {
  r=renderer;
  f=new font;
  f->setrenderer(r);
  #ifdef __linux__
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
  tempc.r=16;
  tempc.g=16;
  tempc.b=16;
  tempc.a=255;
  spath=drawButton(r,0,0,600,20,tempc,4);
  tempc.r=64;
  tempc.g=64;
  tempc.b=64;
  tempc.a=255;
  sload=drawButton(r,0,0,40,20,tempc,4);
}

void Sampler::drawUI() {
  tempc.r=255;
  tempc.g=255;
  tempc.b=255;
  tempc.a=255;
  /*
  for (int i=0; i<v.size(); i++) {
    f->drawf(0,i*16,{255,255,255,255},0,0,"%d: %f",i,v[i].f);
  }
    f->draw(256,32,{255,255,255,255},0,0,0,"Load");
  SDL_SetRenderDrawColor(r,255,255,255,255);
  for (int i=0; i<400; i++) {
    SDL_RenderDrawPoint(r,i,250+s[0].data[(int)(((float)i/400)*(float)s[0].len)]*128);
  }
  SDL_SetRenderDrawColor(r,0,0,0,255);
  */
  SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
  tempr.x=10;  tempr1.x=0;
  tempr.y=10;  tempr1.y=0;
  tempr.w=720; tempr1.w=720;
  tempr.h=340; tempr1.h=340;
  SDL_RenderCopy(r,grid,&tempr1,&tempr);
  tempr.x=0;
  tempr.y=0;
  tempr.w=128;
  tempr.h=64;
  //SDL_RenderCopy(r,but,NULL,&tempr);
  // draw currently playing notes
  SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_ADD);
  SDL_SetRenderDrawColor(r,255,255,255,32);
  for (int i=0; i<v.size(); i++) {
    tempr.x=v[i].note*4;
    tempr.y=10;
    tempr.w=4;
    tempr.h=340;
    SDL_RenderFillRect(r,&tempr);
  }
  SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
  f->draw(370,16,tempc,1,0,0,"Note");
  f->draw(0,200,tempc,1,1,0,"NoteVol");
  f->draw(10,360,tempc,0,0,0,"Sample");
  tempr.x=80;  tempr1.x=0;
  tempr.y=360; tempr1.y=0;
  tempr.w=600; tempr1.w=600;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,spath,&tempr1,&tempr);
  tempr.x=690; tempr1.x=0;
  tempr.y=360; tempr1.y=0;
  tempr.w=40;  tempr1.w=40;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,sload,&tempr1,&tempr);
  f->draw(83,360,tempc,0,0,0,s[0].path);
  f->draw(710,360,tempc,1,0,0,"Load");
  SDL_SetRenderDrawColor(r,(mouse.b[0])?(0):(255),(mouse.b[1])?(0):(255),(mouse.b[2])?(0):(255),255);
  SDL_RenderDrawLine(r,mouse.x,mouse.y,0,0);
  SDL_SetRenderDrawColor(r,0,0,0,255);
  
}

bool Sampler::init(int inChannels, int outChannels) {
  if (inChannels==0) {
    sample=new float[outChannels];
    outCh=outChannels;
    /*v.resize(16);
    for (int i=0; i<v.size(); i++) {
      v[i].period=0;
    }*/
    
    s.resize(1);
    s[0].path="../share/orchestrack/testsmp.wav";
    sndf=sf_open(s[0].path.c_str(),SFM_READ,&si);
    s[0].len=si.frames;
    s[0].chan=si.channels;
    s[0].rate=si.samplerate;
    s[0].data=new float[si.frames*si.channels];
    sf_read_float(sndf,s[0].data,si.frames);
    sf_close(sndf);
    return true;
  } else {
    return false;
  }
}
