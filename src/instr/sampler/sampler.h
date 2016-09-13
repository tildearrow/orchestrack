#include "../../includes.h"
#include "../instr.h"
#include "../../font/font.h"
#include "../../ui/button.h"
#include <sndfile.h>

class Sampler: public OTrackInstrument {
  struct channel {
    int pitch;
    short ctrl[128];
    int pressure;
  };
  struct voice {
    int chan;
    int note;
    int pos;
    int pressure;
    float period;
    float f;
    float vol;
  };
  struct smp {
    string path;
    int len;
    int chan;
    float rate;
    float* data;
  };
  struct {
    int x;
    int y;
    bool b[4];
  } mouse;
  font* f;
  channel c[16];
  std::vector<voice> v;
  std::vector<smp> s;
  unsigned char* ev;
  SNDFILE* sndf;
  SF_INFO si;
  SDL_Texture* but;
  SDL_Texture* grid;
  SDL_Texture* spath;
  SDL_Texture* sload; int sloadS;
  SDL_Texture* scancel;
  SDL_Texture* sloadform;
  SDL_Texture* slflist;
  SDL_Texture* slfdir;
  SDL_Texture* slfpath;
  SDL_Rect tempr, tempr1;
  SDL_Color tempc;
  bool showLoad;
  void drawLoadUI();
  public:
    OTrackInsSpec* getspec();
    float* getSample();
    void setRenderer(SDL_Renderer* renderer);
    void mouseEvent(int type, int button, int x, int y, int finger);
    void drawUI();
    bool init(int inChannels, int outChannels);
};
