#include "engine.h"

int OTrackEngine::init() {
  si=new float[256];
  so=new float[256];
  sichans=0;
  sochans=0;
  for (int i=0; i<256; i++) {
    si[i]=0;
    so[i]=0;
  }
  mi=new std::queue<void*>[256];
  mo=new std::queue<void*>[256];
  pmo=new std::queue<void*>[256];
  me=new unsigned char*[4096]; // should be enough
  for (int i=0; i<4096; i++) {
    me[i]=new unsigned char[16]; // should be enough
  }
  michans=0;
  mochans=0;
  iii=0;
  // for now //
  tempo=125;
  speed=6;
  mult=1;
  subtick=0;
  tick=0;
  row=-1;
  // init empty project //
  p=new OTrackProject;
  // for now too //
  /*
  p->chan=8;
  p->pat.resize(8);
  for (int i=0; i<p->chan; i++) {
    p->pat[i].resize(1);
    p->pat[0][0].len=64;
  }
  p->pat[0][0].ev.resize(1);
  p->pat[0][0].ev[0].time=0;
  p->pat[0][0].ev[0].ticko=0;
  p->pat[0][0].ev[0].note=0x3f;
  p->pat[0][0].ev[0].instr=1;
  p->pat[0][0].ev[0].vol=1.0;
  */
  return 1;
}

int OTrackEngine::addChannel(bool input) {
  if (input) {
    if (sichans>=256) {
      return 0;
    }
    sichans++;
  } else {
    if (sochans>=256) {
      return 0;
    }
    sochans++;
  }
  return 1;
}

int OTrackEngine::addMChannel(bool input) {
  if (input) {
    if (michans>=256) {
      return 0;
    }
    michans++;
  } else {
    if (mochans>=256) {
      return 0;
    }
    mochans++;
  }
  return 1;
}
