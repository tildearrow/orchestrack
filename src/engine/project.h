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
  effval value;
};

struct OTrackEvent {
  int time, note, instr, vol;
  OTrackEffect effect[8];
};

struct OTrackPat {
  int len;
  std::vector<OTrackEvent> ev;
};

struct OTrackSong {
  std::vector<OTrackPat> pat;
};

struct OTrackPInstr {
  OTrackInstrument* i;
  SDL_Texture* ui;
  SDL_Rect bound;
  bool inborder;
};

class OTrackProject {
  public:
    std::vector<OTrackPInstr> ins;
    std::vector<OTrackSong> song;
};
