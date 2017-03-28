#include "engine.h"

void OTrackEngine::updatePos() {
  if ((--subtick)<=0) {
    if ((--tick)<=0) {
      row++;
      tick=speed;
      if (row>=64) {
        pat++;
        printf("next pat.\n");
        row=0;
      }
      printf("row %d\n",row);
      if (testnote[row]) {
        me[0][0]=0x80;
        me[0][1]=cn;
        me[0][2]=0x7f;
        me[1][0]=0x90;
        me[1][1]=testnote[row];
        me[1][2]=0x7f;
        cn=testnote[row];
        pmo[0].push(me[0]);
        pmo[0].push(me[1]);
      }
    }
    subtick+=44100/(tempo/2.5);
    //printf("subtick: %g\n",subtick);
  }
}

float* OTrackEngine::getSample() {
  float* ts;
  unsigned char* me;
  iii++;
  // for now
  so[0]=0;
  so[1]=0;
  updatePos();
  for (int i=0; i<p->ins.size(); i++) {
    while (!pmo[0].empty()) {
      me=(unsigned char*)pmo[0].front();
      pmo[0].pop();
      p->ins[i].i->submitEvent(me);
    }
    ts=p->ins[i].i->getSample();
    so[0]+=ts[0];
    so[1]+=ts[1];
  }
  return so;
}
