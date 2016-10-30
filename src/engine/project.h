#include "../includes.h"
//#include "../instr/instr.h"

struct OTrackEffect {
  int type, value;
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
