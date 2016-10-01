#include "../../includes.h"
#include "../instr.h"
#include "../../font/font.h"
#include "../../ui/button.h"
#include "../../format/wave.h"
#include "../../engine/lutgen.h"
#include <sndfile.h>
#include <assert.h>

#define fShift (16-(WFIR_FRACBITS+1+WFIR_LOG2WIDTH))
#define fMask ((((1L<<(17-fShift))-1)&~((1L<<WFIR_LOG2WIDTH)-1)))
#define fHalve (1L<<(16-(WFIR_FRACBITS+2)))

class Sampler: public OTrackInstrument {
  // channel structure //
  struct channel {
    int pitch;
    short ctrl[128];
    int pressure;
  };
  // voice structure //
  struct voice {
    int chan;
    int note;
    int pos;
    int pressure;
    int sample;
    long periodN;
    float periodD;
    float f;
    float vol;
    int env;
    int envpos;
  };
  // sample structure //
  struct smp {
    string path;
    int len;
    int chan;
    char noteMin, noteMax;
    char velMin, velMax;
    float rate;
    float** data;
    int loopStart;
    int loopEnd;
    int loopType;
  };
  // mouse structure //
  struct {
    int x;
    int y;
    bool b[4];
  } mouse;
  // directory entry structure //
  struct dentry {
    string name;
    int type;
    int size;
  };
  // list entry structure //
  struct listentry {
    string name;
    string rh;
    SDL_Color color;
  };
  // envelope point structure //
  struct envp {
    float time;
    float value;
    int type;
  };
  // envelope structure //
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
  // vectors //
  std::vector<voice> v;
  std::vector<envl> e;
  std::vector<smp> s;
  std::vector<dentry> listings;
  std::vector<listentry> listelem;
  // list stuff //
  string wd;
  int wdoff;
  int erra;
  string errl[3];
  bool wddir;
  string sfname;
  unsigned char* ev;
  float* tbuf;
  // sinc table //
  float table[WFIR_LUTLEN*WFIR_WIDTH];
  // for file loading //
  SNDFILE* sndf;
  iff* lf;
  rwave* lwf;
  SF_INFO si;
  // textures //
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
  // eye candy //
  float aBBPos;
  // temporary SDL stuff //
  SDL_Rect tempr, tempr1;
  SDL_Color tempc;
  // internal switches //
  bool showLoad;
  bool showSampleSel;
  bool showHidden;
  // busy indicator //
  bool busy;
  // list variables //
  float listPos;
  float fingerS;
  float listSpeed;
  bool listDir;
  bool scrolling;
  float touchSPos;
  float polledMY, oldPolledMY;
  bool touching;
  // real-time MIDI tick //
  long tick;
  // more list variables //
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
  // accel button variables //
  bool doUp, doDown; int doXTarget;
  int timeOnButton;
  // 0: drawSummary
  // 1: drawGrid
  // 2: drawSampleEdit
  // 3: drawEnvEdit
  // main UI variables //
  int curView;
  // sample edit variables //
  int curSample;
  // keypad variables //
  string kpCurVal;
  float* kpVar;
  // interpolation methods //
  inline float intNone(float* b, int n, float d);
  inline float intLinear(float* b, int n, float d);
  inline float intCubic(float* b, int n, float d);
  inline float intSinc(float* b, int n, float d);
  // error string functions //
  string friendlyErr0(int e);
  string friendlyErr1(int e);
  string friendlyErr2(int e);
  // check for hover //
  void hover(int x, int y, int x2, int y2, int* result);
  // draw keypad //
  void drawKeypad();
  // accel button update //
  void upDown();
  // down/up/move //
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
  void grMouseDown(int button);
  void grMouseUp(int button);
  void grMouseMove(int button);
  void seMouseDown(int button);
  void seMouseUp(int button);
  void seMouseMove(int button);
  // loading functions //
  void loadSample();
  // list functions //
  void clearList();
  void feedList(string name, string rh, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
  void drawList();
  void drawLoadUI();
  void prepareSampleSel();
  // draw UI functions //
  void drawSampleSel();
  void drawSummary();
  void drawGrid();
  void drawEnvEdit();
  void drawSampleEdit();
  // directory functions //
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
