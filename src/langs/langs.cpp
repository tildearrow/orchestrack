#include "langs.h"

#ifdef _WIN32
#define _ds "\\"
#else
#define _ds "/"
#endif

std::string* lsLoad(const char* dir, const char* lang) {
  FILE* f;
  char* fn;
  std::string* ret;
  int c, ci, s, t;
  if (dir==NULL || lang==NULL) {
    return 0;
  }
  fn=new char[strlen(dir)+strlen(lang)+6];
  memcpy(fn,dir,strlen(dir));
  fn[strlen(dir)]=_ds;
  memcpy(fn+1+strlen(dir),lang,strlen(lang));
  strcpy(fn+1+strlen(dir)+strlen(lang),".tsv");
  if (!(f=fopen(fn,"rb"))) {
    return 0;
  }
  delete[] fn;
  while (!feof(f)) {
    if (fgetc(f)=='\n') {
      t++;
    }
  }
  fseek(f,0,SEEK_SET);
  ret=new std::string[t];
  s=0;
  ci=0;
  while (!feof(f)) {
    c=fgetc(f);
    switch (c) {
      case '\t':
        s=1;
        break;
      case '\n':
        s=0; ci++;
        break;
      default:
        if (s) ret[ci]+=c;
        break;
    }
  }
  fclose(f);
  return ret;
}
