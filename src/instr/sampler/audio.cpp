#include "sampler.h"
// TODO: "magic" release mode:
//       scale envelope content past sustain to match current volume on
//       note off

void Sampler::updateEnv(envl* envel, int* posN, float* posD, int* envpi, voice* object) {
  if (envel!=NULL) {
    posD[0]+=65536/44100;
    if (envel->susStart!=envpi[0] || envel->susStart!=envel->susEnd || envel->susEnd==-1 || object->released) {
      posN[0]+=(int)posD[0];
    }
    posD[0]-=(int)posD[0];
    if ((posN[0]+envel->p[envpi[0]].time)>envel->p[envpi[0]+1].time) {
      if (envpi[0]==envel->susEnd-1 && !object->released) {
        envpi[0]=envel->susStart;
      } else {
        envpi[0]++;
      }
      posN[0]=0;
    }
  }
}

float* Sampler::getSample() {
  abusy=true;
  size_t i, j;
  float calc, pitchcalc;
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
            if (v[i].envVol==NULL) {
              vErase(i); i--;
            } else if (v[i].envVol->susStart==-1) {
              vErase(i); i--;
            } else {
              v[i].released=true;
              if (v[i].envVol->relMode) {
                // jump, or "magic" mode
                v[i].envVpi=v[i].envVol->susEnd;
                v[i].envVposN=0;
              }
            }
          }
        }
      }
    }
    if ((ev[0]>>4)==9) {
      // allocate a voice
      vResize(vSize+1);
      int thisv;
      thisv=vSize-1;
      v[thisv].released=false;
      v[thisv].chan=ev[0]&15;
      v[thisv].note=ev[1];
      v[thisv].periodN=0;
      v[thisv].periodD=0;
      v[thisv].sample=NULL;
      for (i=0; i<sSize; i++) {
        if (s[i].noteMin<=v[thisv].note && s[i].noteMax>=v[thisv].note &&
            s[i].velMin<=ev[2] && s[i].velMax>=ev[2]) {
          v[thisv].sample=&s[i]; v[thisv].samplen=i; break;
        }
      }
      if (v[thisv].sample==NULL) {
        vErase(thisv);
      } else {
        v[thisv].f=pow(2.0f,((float)v[thisv].note-60.0f)/12.0f)*v[thisv].sample->rate/44100.0f;
        v[thisv].vol=(float)ev[2]/128.0f;
        if (v[thisv].sample->envVol==-1) {
          v[thisv].envVol=NULL;
        } else {
          v[thisv].envVol=&e[v[thisv].sample->envVol];
        }
        if (v[thisv].sample->envPitch==-1) {
          v[thisv].envPitch=NULL;
        } else {
          v[thisv].envPitch=&e[v[thisv].sample->envPitch];
        }
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
    //printf("%d %d\n",object->env,object->envVpi);
    // volume
    if (object->envVol==NULL) {
      calc=object->vol*(object->sample->volAmt+object->sample->volCap);
    } else {
      val0=object->envVol->p[object->envVpi].value;
      val1=object->envVol->p[object->envVpi+1].value;
      timediff=object->envVol->p[object->envVpi+1].time-object->envVol->p[object->envVpi].time;
      calc=object->vol*(object->sample->volAmt*(val0+((val1-val0)*(1.0f-(timediff-(float)object->envVposN)/timediff)))+object->sample->volCap);
    }
    // pitch
    if (object->envPitch==NULL) {
      pitchcalc=object->f*(object->sample->pitchAmt+object->sample->pitchCap);
    } else {
      val0=object->envPitch->p[object->envPipi].value;
      val1=object->envPitch->p[object->envPipi+1].value;
      timediff=object->envPitch->p[object->envPipi+1].time-object->envPitch->p[object->envPipi].time;
      pitchcalc=object->vol*(object->sample->pitchAmt*(val0+((val1-val0)*(1.0f-(timediff-(float)object->envPiposN)/timediff)))+object->sample->pitchCap);
    }
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
    object->periodD+=pitchcalc;
    object->periodN+=(int)object->periodD;
    if (object->sample->loopType==1 && object->periodN>object->sample->loopEnd) {
      object->periodN=object->sample->loopStart+(object->periodN%(object->sample->loopEnd+1));
    }
    object->periodD-=(int)object->periodD;
    
    if (object->envVol!=NULL) {
      updateEnv(object->envVol,&object->envVposN,&object->envVposD,&object->envVpi,object);
      if (object->envVpi==(object->envVol->pSize-1)) {
        vErase(i); i--;
        printf("end of envelope.\n");
      }
    }
    updateEnv(object->envPitch,&object->envPiposN,&object->envPiposD,&object->envPipi,object);
    
    if ((int)object->periodN>object->sample->len) {
      vErase(i); i--;
    }
  }
  
  sample[0]=sample[0]/4;
  sample[1]=sample[1]/4;
  abusy=false;
  return sample;
}
