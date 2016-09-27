#include "../includes.h"
extern "C" {
  #include "../file/fextra.h"
}

struct riff {
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
};

riff* readRIFF(FILE* f);
