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
  sample[0]=0;
  sample[1]=0;
  ev=(unsigned char*)getEvent();
  if (ev!=NULL) {
    if ((ev[0]>>4)==8) {
      v[ev[0]&15].period=0;
      v[ev[0]&15].f=0;
      v[ev[0]&15].vol=(float)ev[2]/128;
    }
    if ((ev[0]>>4)==9) {
      v[ev[0]&15].period=0;
      v[ev[0]&15].f=440*pow(2,((float)ev[1]-69)/12);
      v[ev[0]&15].vol=(float)ev[2]/128;
    }
  }
  for (int i=0; i<v.size(); i++) {
    sample[0]+=(float)v[i].period*v[i].vol;
    sample[1]+=(float)v[i].period*v[i].vol;
    v[i].period=fmod(v[i].period+v[i].f/44100,1);
  }
  sample[0]=sample[0]/4;
  sample[1]=sample[1]/4;
  return sample;
}

void Sampler::setRenderer(SDL_Renderer* renderer) {
  r=renderer;
  f=new font;
  f->setrenderer(r);
  f->load("/usr/share/fonts/TTF/Ubuntu-R.ttf",20);
}

void Sampler::drawUI() {
  for (int i=0; i<v.size(); i++) {
    f->drawf(0,i*16,{255,255,255,255},0,0,"%d: %f",i,v[i].f);
  }
}

bool Sampler::init(int inChannels, int outChannels) {
  if (inChannels==0) {
    sample=new float[outChannels];
    outCh=outChannels;
    v.resize(16);
    for (int i=0; i<v.size(); i++) {
      v[i].period=0;
    }
    
    return true;
  } else {
    return false;
  }
}
