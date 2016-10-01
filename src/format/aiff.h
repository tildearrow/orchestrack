#include "iff.h"

struct aiff {
  struct marker {
    int id;
    long pos;
    std::string name;
  };
  std::vector<marker> m;
  struct loop {
    short mode;
    int start, end;
  };
  struct {
    char note, detune, noteMin, noteMax, velMin, velMax;
    short gain;
    loop sloop;
    loop rloop;
  } inst;
};

aiff* readAIFF(iff* f);

void freeAIFF(aiff* r);
