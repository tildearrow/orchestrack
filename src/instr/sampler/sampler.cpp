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
  if (ev!=NULL) {
    if ((ev[0]>>4)==8) {
      v[ev[0]&15].period=0;
      v[ev[0]&15].f=0;
      v[ev[0]&15].vol=(float)ev[2]/128;
    }
    if ((ev[0]>>4)==9) {
      v[ev[0]&15].period=0;
      v[ev[0]&15].f=pow(2,((float)ev[1]-60)/12)*s[0].rate/44100;
      v[ev[0]&15].vol=(float)ev[2]/128;
    }
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
}

bool Sampler::init(int inChannels, int outChannels) {
  if (inChannels==0) {
    sample=new float[outChannels];
    outCh=outChannels;
    v.resize(16);
    for (int i=0; i<v.size(); i++) {
      v[i].period=0;
    }
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
