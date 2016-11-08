#include "sampler.h"

bool Sampler::loadState(FILE* data) {
  // for now display an error
  erra=64;
  errl[0]=friendlyErr0(0);
  errl[1]=friendlyErr1(0);
  errl[2]=friendlyErr2(0);
  return false;
}

unsigned char* Sampler::saveState(int* size) {
  FILE* fi;
  int siz, siztw;
  unsigned char* ret;
  if (!(fi=tmpfile())) {
    return NULL;
  }
  // header chunk //
  fputc('h',fi);
  fputc('e',fi);
  fputc('a',fi);
  fputc('d',fi);
  fputi(20,fi); // size
  // see doc/instr/dev/format.md
  fputc(0,fi); // audio in
  fputc(2,fi); // audio out (for now)
  fputc(1,fi); // midi in
  fputc(0,fi); // midi out
  fputs("OTRKSMP",fi); // identifier
  fputs("Sampler",fi); // name (for now)
  // sample header //
  // see doc/instr/sampler/tech.md
  fputc('s',fi);
  fputc('m',fi);
  fputc('p',fi);
  fputc('h',fi);
  fputc(36,fi); // the header is exactly 36 bytes
  fputc(0,fi);
  fputc(0,fi);
  fputc(0,fi);
  fputi(1,fi); // version 1
  fputs("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",fi); // padding for now...
  fputi(sSize,fi); // sample count
  fputi(eSize,fi); // env count
  fputi(0,fi); // coming soon?
  // samples //
  for (int i=0; i<sSize; i++) {
    fputc('s',fi);
    fputc('m',fi);
    fputc('p',fi);
    fputc('s',fi);
    fputi(0,fi); // skip for now, size will be written later
    siz=ftell(fi);
    fputi(i,fi); // number
    fputs(s[i].path->c_str(),fi); // name
    fputf(s[i].rate,fi); // rate
    fputi(0,fi); // padding
    fputc(s[i].noteMin,fi);
    fputc(s[i].noteMax,fi);
    fputc(s[i].velMin,fi);
    fputc(s[i].velMax,fi);
    fputi(0,fi); // padding, because i may
    fputi(0,fi); // add something later
    fputi(0,fi); 
    // volume env
    fputi(s[i].envVol,fi);
    fputf(s[i].volAmt,fi);
    fputf(s[i].volCap,fi);
    fputi(0,fi);
    // panning env
    fputi(s[i].envPan,fi);
    fputf(s[i].panAmt,fi);
    fputf(s[i].panCap,fi);
    fputi(0,fi);
    // pitch env
    fputi(s[i].envPitch,fi);
    fputf(s[i].pitchAmt,fi);
    fputf(s[i].pitchCap,fi);
    fputi(0,fi);
    // cutoff env
    fputi(s[i].envCut,fi);
    fputf(s[i].cutAmt,fi);
    fputf(s[i].cutCap,fi);
    fputi(0,fi);
    // resonance env
    fputi(s[i].envRes,fi);
    fputf(s[i].resAmt,fi);
    fputf(s[i].resCap,fi);
    fputi(0,fi);
    // filter, and others
    fputc(s[i].filter,fi);
    fputc(0,fi); // more padding
    fputc(0,fi);
    fputc(0,fi);
    fputi(s[i].loopStart,fi);
    fputi(s[i].loopEnd,fi);
    fputi(s[i].loopType,fi);
    fputi(s[i].len,fi);
    fputi(s[i].chan,fi);
    // TODO:
    // - flags
    // - remote flag
    fputi(0,fi);
    // sample data
    for (int j=0; j<(s[i].len*s[i].chan); j++) {
      fputf(s[i].data[j%s[i].chan][j/s[i].chan],fi);
    }
    // write size
    siztw=ftell(fi)-siz;
    fseek(fi,siz-4,SEEK_SET);
    fputi(siztw,fi);
    fseek(fi,0,SEEK_END);
  }
  // envelopes //
  for (int i=0; i<eSize; i++) {
    fputc('s',fi);
    fputc('m',fi);
    fputc('p',fi);
    fputc('e',fi);
    fputi(0,fi); // skip for now, size will be written later
    siz=ftell(fi);
    fputi(i,fi); // number
    fputs(e[i].name->c_str(),fi); // name
    fputi(e[i].start,fi);
    fputi(e[i].susStart,fi);
    fputi(e[i].susEnd,fi);
    fputi(e[i].loopStart,fi);
    fputi(e[i].loopEnd,fi);
    fputi(65536,fi); // precision
    fputi(e[i].start,fi); // TODO: flags
    fputi(e[i].pSize,fi); // point count
    // points
    for (int j=0; j<e[i].pSize; j++) {
      fputi(e[i].p[j].time,fi);
      fputf(e[i].p[j].value,fi);
      fputi(0,fi);
      fputf(0.0f,fi);
    }
    // write size
    siztw=ftell(fi)-siz;
    fseek(fi,siz-4,SEEK_SET);
    fputi(siztw,fi);
    fseek(fi,0,SEEK_END);
  }
  siz=ftell(fi);
  fseek(fi,4,SEEK_SET);
  fputi(siz,fi);
  ret=new unsigned char[siz];
  fseek(fi,0,SEEK_SET);
  fread(ret,1,siz,fi);
  if (size!=NULL) {
    *size=siz;
  }
  fclose(fi);
  return ret;
  // TODO: sizes, and actually saving. good night.
}
