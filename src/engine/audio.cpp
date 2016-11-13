#include "engine.h"

float* OTrackEngine::getSample() {
  float* ts;
  unsigned char* me;
  me=new unsigned char[3];
  me[0]=0x90;
  me[1]=0x60;
  me[2]=0x7f;
  iii++;
  // for now
  so[0]=0;
  so[1]=0;
  for (int i=0; i<p.ins.size(); i++) {
    if (iii==200000) {
      p.ins[i].i->submitEvent(me);
    }
    ts=p.ins[i].i->getSample();
    so[0]+=ts[0];
    so[1]+=ts[1];
  }
  delete[] me;
  return so;
}
