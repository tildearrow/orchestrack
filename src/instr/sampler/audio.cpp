#include "sampler.h"

float* Sampler::getSample() {
  size_t i, j;
  float calc;
  if (busy) {vResize(0); return sample;}
  float element;
  float val0, val1, timediff;
  sample[0]=0;
  sample[1]=0;
  ev=(unsigned char*)getEvent();
  while (ev!=NULL) {
    if ((ev[0]>>4)==8) {
      // find voice with properties, then destroy it
      for (i=0; i<vSize; i++) {
        if (v[i].chan==(ev[0]&15)) {
          if (v[i].note==ev[1]) {
            vErase(i); i--;
          }
        }
      }
    }
    if ((ev[0]>>4)==9) {
      // allocate a voice
      vResize(vSize+1);
      int thisv;
      thisv=vSize-1;
      v[thisv].chan=ev[0]&15;
      v[thisv].note=ev[1];
      v[thisv].periodN=0;
      v[thisv].periodD=0;
      v[thisv].sample=NULL;
      for (i=0; i<sSize; i++) {
        if (s[i].noteMin<=v[thisv].note && s[i].noteMax>=v[thisv].note &&
            s[i].velMin<=ev[2] && s[i].velMax>=ev[2]) {
          v[thisv].sample=&s[i]; break;
        }
      }
      if (v[thisv].sample==NULL) {
        vErase(thisv);
      } else {
      v[thisv].f=pow(2.0f,((float)v[thisv].note-60.0f)/12.0f)*v[thisv].sample->rate/44100.0f;
      v[thisv].vol=(float)ev[2]/128.0f;
      v[thisv].envVol=&e[0];
      }
    }
    if ((ev[0]>>4)==0xe) {
      // pitch bend.
      c[ev[0]&15].pitch=(ev[1]+(ev[2]<<7))-0x2000;
      for (i=0; i<vSize; i++) {
        if (v[i].chan==(ev[0]&15)) {
          v[i].f=pow(2.0f,((float)v[i].note-60+((float)c[ev[0]&15].pitch/4096.0f))/12)*v[i].sample->rate/44100;
        }
      }
    }
    if ((ev[0]>>3)==0x1e) {
      // SC messages
      switch (ev[0]&15) {
        case 2: tick=ev[1]+(ev[1]<<7); printf("pos\n"); break;
      }
    }
    if ((ev[0]>>3)==0x1f) {
      // real-time messages.
      switch (ev[0]&15) {
        case 8: tick++; break;
        case 0xa: tick=0; break;
        case 0xf: reset(); break;
      }
    }
    ev=(unsigned char*)getEvent();
  }
  for (i=0; i<vSize; i++) {
    voice* object=&v[i];
    //printf("%d %d\n",object->env,object->envpi);
    val0=object->envVol->p[object->envpi].value;
    val1=object->envVol->p[object->envpi+1].value;
    timediff=object->envVol->p[object->envpi+1].time-object->envVol->p[object->envpi].time;
    calc=object->vol*(object->sample->volAmt*(val0+((val1-val0)*(1.0f-(timediff-(float)object->envposN)/timediff)))+object->sample->volCap);
    if (object->sample->chan==1) {
      float elcalc;
      element=intSinc(object->sample->data[0],object->periodN+8,object->periodD);
      
      elcalc=element*calc;
      sample[0]+=elcalc;
      sample[1]+=elcalc;
    } else for (j=0; j<(size_t)object->sample->chan; j++) {
      element=intSinc(object->sample->data[j],object->periodN+8,object->periodD);
      
      sample[j]+=element*calc;
    }
    object->periodD+=object->f;
    object->periodN+=(int)object->periodD;
    if (object->sample->loopType==1 && object->periodN>object->sample->loopEnd) {
      object->periodN=object->sample->loopStart+(object->periodN%(object->sample->loopEnd+1));
    }
    object->periodD=fmod(object->periodD,1.0f);
    object->envposD+=65536/44100;
    object->envposN+=(int)object->envposD;
    object->envposD=fmod(object->envposD,1.0f);
    if ((object->envposN+object->envVol->p[object->envpi].time)>object->envVol->p[object->envpi+1].time) {
      object->envpi++;
      object->envposN=0;
      if (object->envpi==(object->envVol->pSize-1)) {
        vErase(i); i--;
        printf("end of envelope.\n");
      }
    }
    if ((int)object->periodN>object->sample->len) {
      vErase(i); i--;
    }
  }
  
  sample[0]=sample[0]/4;
  sample[1]=sample[1]/4;
  return sample;
}
