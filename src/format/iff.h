#ifndef _IFF_H
#define _IFF_H
#include "../includes.h"
extern "C" {
  #include "../file/fextra.h"
}

struct iff {
  struct header {
    char id[5];
    long size;
    char format[5];
  } h;
  struct subchnk {
    char id[5];
    long size;
    unsigned char* data;
  };
  std::vector<subchnk> s;
  bool isRIFF;
};

iff* readIFF(FILE* f);

void freeIFF(iff* r);
#endif