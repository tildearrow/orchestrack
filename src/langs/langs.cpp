#include "langs.h"

#ifdef _WIN32
#define _ds "\\"
#else
#define _ds "/"
#endif

std::string* lsLoad(const char* dir, const char* lang) {
  FILE* f;
  char* fn;
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
}
