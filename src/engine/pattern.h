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

class OTrackProject {

};
