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

void Sampler::setRenderer(SDL_Renderer* renderer) {
  r=renderer;
  f=new font;
  f->setrenderer(r);
  f->load("/usr/share/fonts/TTF/Ubuntu-R.ttf",16);
}

void Sampler::drawUI() {
  //drawButton(r,0,0,256,256,{128,128,128,255},16);
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
  f->drawf(0,16,{255,255,255,255},0,0,"%d",v.size());
}

bool Sampler::init(int inChannels, int outChannels) {
  if (inChannels==0) {
    sample=new float[outChannels];
    outCh=outChannels;
    /*v.resize(16);
    for (int i=0; i<v.size(); i++) {
      v[i].period=0;
    }*/
    sndf=sf_open("../share/orchestrack/testsmp.wav",SFM_READ,&si);
    s.resize(1);
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
