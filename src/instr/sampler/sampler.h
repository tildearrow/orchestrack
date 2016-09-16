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
    float** data;
  };
  struct {
    int x;
    int y;
    bool b[4];
  } mouse;
  struct dentry {
    string name;
    int type;
  };
  font* f;
  channel c[16];
  std::vector<voice> v;
  std::vector<smp> s;
  std::vector<dentry> listings;
  string wd;
  string sfname;
  unsigned char* ev;
  float* tbuf;
  SNDFILE* sndf;
  SF_INFO si;
  SDL_Texture* but;
  SDL_Texture* grid;
  SDL_Texture* spath;
  SDL_Texture* sload; int sloadS; int supS; int seupS; int sedownS;
  SDL_Texture* scancel; int scancelS;
  SDL_Texture* sloadform;
  SDL_Texture* slflist;
  SDL_Texture* slfdir;
  SDL_Texture* slfpath;
  SDL_Texture* smode;
  SDL_Texture* smodeactive;
  SDL_Rect tempr, tempr1;
  SDL_Color tempc;
  bool showLoad;
  bool showHidden;
  bool busy;
  float listPos;
  float fingerS;
  float listSpeed;
  bool listDir;
  bool scrolling;
  float touchSPos;
  float polledMY, oldPolledMY;
  bool touching;
  long tick;
  int loadHIndex, loadSIndex;
  bool doUp, doDown;
  int timeOnButton;
  // 0: drawSummary
  // 1: drawGrid
  // 2: drawSampleEdit
  // 3: drawEnvEdit
  int curView;
  string kpCurVal;
  float* kpVar;
  void hover(int x, int y, int x2, int y2, int* result);
  void drawKeypad();
  void kpMouseDown(int button);
  void kpMouseUp(int button);
  void kpMouseMove(int button);
  void loadMouseDown(int button);
  void loadMouseUp(int button);
  void loadMouseMove(int button);
  void seMouseDown(int button);
  void seMouseUp(int button);
  void seMouseMove(int button);
  void drawLoadUI();
  void drawSummary();
  void drawGrid();
  void drawEnvEdit();
  void drawSampleEdit();
  string topLevel(string path);
  int readDir(const char* path);
  public:
    OTrackInsSpec* getspec();
    float* getSample();
    void setRenderer(SDL_Renderer* renderer);
    void mouseEvent(int type, int button, int x, int y, int finger);
    void drawUI();
    void reset();
    bool init(int inChannels, int outChannels);
};
