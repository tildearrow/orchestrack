#include "../includes.h"
#include "../instr/instr.h"

union effval {
  long long int sixtyfour;
  double dbl;
  float f[2];
  int i[2];
  short s[4];
  char sb[8];
  unsigned char ub[8];
};

struct OTrackEffect {
  int type;
  int time;
  effval value;
};

struct OTrackEvent {
  int time, ticko, note, instr;
  float vol;
  OTrackEffect effect[8];
};

struct OTrackPat {
  int len;
  std::vector<OTrackEvent> ev;
};

struct OTrackOrder {
  int pat, trans;
};

struct OTrackChannel {
  std::vector<OTrackPat> pat;
  std::vector<OTrackOrder> ord;
};

struct OTrackPInstr {
  OTrackInstrument* i;
  SDL_Texture* ui;
  SDL_Rect bound;
  bool inborder;
};

class OTrackBlock {
  int ai, ao, mi, mo;
  float*(*aof)(void);
  int(*aif)(float*);
  unsigned char* getMEvent(int channel);
  int putMEvent(int channel, void* data);
};

class OTrackProject {
  public:
    std::vector<OTrackPInstr> ins;
    int chan;
    std::vector<OTrackChannel> pat;
};
