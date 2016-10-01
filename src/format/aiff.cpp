#include "aiff.h"

aiff* readAIFF(iff* f) {
  aiff* r;
  r=new aiff;
  /*
  r->smpl.loops=0;
  r->smpl.l.resize(0);
   */
  for (int i=0; i<f->s.size(); i++) {
    if (strcmp(f->s[i].id,"MARK")==0) {
      printf("marker chunk!\n");
      r->m.resize((f->s[i].data[0]<<8)+(f->s[i].data[1]));
      printf("markers: %lu\n",r->m.size());
      int curpos;
      curpos=2;
      for (int j=0; j<r->m.size(); j++) {
        int nameSize;
        r->m[j].id=(f->s[i].data[curpos++]<<8)+(f->s[i].data[curpos++]);
        r->m[j].pos=(f->s[i].data[curpos++]<<24)+(f->s[i].data[curpos++]<<16)+
                   (f->s[i].data[curpos++]<<8)+(f->s[i].data[curpos++]);
        nameSize=((f->s[i].data[curpos++]+1)/2)*2;
        for (int k=0; k<nameSize; k++) {
          r->m[j].name+=f->s[i].data[curpos++];
        }
        printf("marker %d:\n",j);
        printf("id %d pos %d name %s\n",r->m[j].id,r->m[j].pos,r->m[j].name.c_str());
      }
    } else if (strcmp(f->s[i].id,"INST")==0) {
      printf("instrument chunk!\n");
      r->inst.note=f->s[i].data[0];
      r->inst.detune=f->s[i].data[1];
      r->inst.noteMin=f->s[i].data[2];
      r->inst.noteMax=f->s[i].data[3];
      r->inst.velMin=f->s[i].data[4];
      r->inst.velMax=f->s[i].data[5];
      r->inst.gain=(f->s[i].data[6]<<8)+(f->s[i].data[7]);
      r->inst.sloop.mode=(f->s[i].data[8]<<8)+(f->s[i].data[9]);
      r->inst.sloop.start=(f->s[i].data[10]<<8)+(f->s[i].data[11]);
      r->inst.sloop.end=(f->s[i].data[12]<<8)+(f->s[i].data[13]);
      r->inst.rloop.mode=(f->s[i].data[14]<<8)+(f->s[i].data[15]);
      r->inst.rloop.start=(f->s[i].data[16]<<8)+(f->s[i].data[17]);
      r->inst.rloop.end=(f->s[i].data[18]<<8)+(f->s[i].data[19]);
      printf("note %d detune %d\n",r->inst.note,r->inst.detune);
      printf("nm %d nM %d\n",r->inst.noteMin,r->inst.noteMax);
      printf("vm %d vM %d\n",r->inst.velMin,r->inst.velMax);
      printf("gain %d\n",r->inst.gain);
      printf("sloop: %d %d %d\n",r->inst.sloop.mode,r->inst.sloop.start,r->inst.sloop.end);
      printf("rloop: %d %d %d\n",r->inst.rloop.mode,r->inst.rloop.start,r->inst.rloop.end);
    } else {
      printf("other chunk!\n");
    }
  }
  return r;
}

void freeAIFF(aiff* r) {
  delete r;
}
