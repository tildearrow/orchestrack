#include "iff.h"

struct rwave {
  struct {
    short aformat;
    short chan;
    long srate;
    long byterate;
    short bal;
    short bps;
    short extras;
    unsigned char* extra;
  } fmt;
  struct {
    long size;
    float* data;
  } data;
  struct cuept {
    long id;
    long dwpos;
    char chunk[5];
    long coff;
    long boff;
    long pos;
  };
  std::vector<cuept> cue;
  struct segment {
    long id, len, rep;
  };
  std::vector<segment> plst;
  struct {
    long id;
    std::string val;
  } labl;
  struct {
    long id;
    std::string val;
  } note;
  struct {
    long id, sl, purpose;
    short country, lang, dial, cp;
    std::string val;
  } ltxt;
  struct loop {
    long id, type, start, end, frac, pc;
  };
  struct {
    long make, prod, speriod, MUN, MPF, Sform, Soff, loops, extral;
    std::vector<loop> l;
  } smpl;
  struct {
    unsigned char unote;
    char ft, gain;
    unsigned char noteMin, noteMax, velMin, velMax;
  } inst;
};

rwave* readWAVE(iff* f);

void freeWAVE(rwave* r);
