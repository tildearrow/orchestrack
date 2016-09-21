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
    int sample;
    float period;
    float f;
    float vol;
    int env;
    int envpos;
  };
  struct smp {
    string path;
    int len;
    int chan;
    char noteMin, noteMax;
    char velMin, velMax;
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
  struct listentry {
    string name;
    SDL_Color color;
  };
  struct envp {
    float time;
    float value;
  };
  struct envl {
    std::vector<envp> p;
    int start;
    int attackEnd;
    int decayEnd;
    int sustainEnd;
    int loopStart;
    int loopEnd;
    bool susLoop;
    bool relMode;
    bool sync;
  };
  font* f;
  channel c[16];
  std::vector<voice> v;
  std::vector<envl> e;
  std::vector<smp> s;
  std::vector<dentry> listings;
  std::vector<listentry> listelem;
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
  SDL_Texture* smode; int smupS[8]; int smdownS[8];
  SDL_Texture* smodeactive;
  SDL_Texture* srange;
  SDL_Texture* srangebutton;
  SDL_Texture* sselect; int sselectS;
  SDL_Rect tempr, tempr1;
  SDL_Color tempc;
  bool showLoad;
  bool showSampleSel;
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
  // doXtarget:
  // 0: rate
  // 1: note min
  // 2: note max
  // 3: volume min
  // 4: volume max
  // 5: param1 min
  // 6: param1 max
  // 7: param2 min
  // 8: param2 max
  bool doUp, doDown; int doXTarget;
  int timeOnButton;
  // 0: drawSummary
  // 1: drawGrid
  // 2: drawSampleEdit
  // 3: drawEnvEdit
  int curView;
  int curSample;
  string kpCurVal;
  float* kpVar;
  void hover(int x, int y, int x2, int y2, int* result);
  void drawKeypad();
  void kpMouseDown(int button);
  void kpMouseUp(int button);
  void kpMouseMove(int button);
  void listMouseDown(int button);
  void listMouseUp(int button);
  void listMouseMove(int button);
  void listMouseWheel(int x, int y);
  void loadMouseDown(int button);
  void loadMouseUp(int button);
  void loadMouseMove(int button);
  void seMouseDown(int button);
  void seMouseUp(int button);
  void seMouseMove(int button);
  void loadSample();
  void clearList();
  void feedList(string name, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
  void drawList();
  void drawLoadUI();
  void prepareSampleSel();
  void drawSampleSel();
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
