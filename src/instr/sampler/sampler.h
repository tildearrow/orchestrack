#include "../../includes.h"
#include "../instr.h"
#include "../../font/font.h"
#include "../../ui/button.h"
#include "../../ui/knob.h"
#include "../../ui/text.h"
#include "../../format/wave.h"
#include "../../format/aiff.h"
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
  // envelope point structure //
  struct envp {
    int time;
    float value;
    int type;
  };
  // envelope structure //
  struct envl {
    envp* p;
    size_t pSize;
    int start;
    int susStart;
    int susEnd;
    int loopStart;
    int loopEnd;
    bool susLoop;
    char relMode;
    bool sync;
    bool persistent;
    string* name;
  };
  // sample structure //
  struct smp {
    string* path;
    int len;
    int chan;
    char noteMin, noteMax;
    char velMin, velMax;
    char filter;
    float rate;
    float** data;
    int loopStart;
    int loopEnd;
    int loopType;
    int envVol;
    int envPan;
    int envPitch;
    int envCut;
    int envRes;
    float volAmt, volCap;
    float panAmt, panCap;
    float pitchAmt, pitchCap;
    float cutAmt, cutCap;
    float resAmt, resCap;
  };
  // voice structure //
  struct voice {
    bool released;
    int chan;
    int note;
    int pos;
    int pressure;
    int samplen;
    smp* sample;
    long periodN;
    float periodD;
    float f;
    float vol;
    int env;
    int envVpi, envPpi, envPipi, envCpi, envRpi;
    int envVposN, envPposN, envPiposN, envCposN, envRposN;
    float envVposD, envPposD, envPiposD, envCposD, envRposD;
    float flow[2], fhigh[2], fband[2];
    envl* envVol;
    envl* envPan;
    envl* envPitch;
    envl* envCut;
    envl* envRes;
  };
  // global parameters //
  float volume;
  float panning;
  float pitch;
  // mouse structure //
  struct {
    int x;
    int y;
    bool b[32];
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
  font* f;
  channel c[16];
  // vectors //
  voice v[256];
  envl e[1024];
  smp* s;
  size_t sSize;
  size_t vSize;
  size_t eSize;
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
  aiff* laf;
  SF_INFO si;
  // textures //
  SDL_Texture* but;
  SDL_Texture* grid;
  SDL_Texture* envbg;
  SDL_Texture* spath;
  SDL_Texture* spathlarge;
  SDL_Texture* spathshort; int sevolS; int sepanS; int sepitchS; int secutS;
                           int seresS;
  SDL_Texture* senvsel;
  SDL_Texture* sload; int sloadS; int supS; int seupS; int sedownS; int slloadS;
  SDL_Texture* scancel; int scancelS;
  SDL_Texture* sloadform;
  SDL_Texture* slflist;
  SDL_Texture* slfdir;
  SDL_Texture* slfpath;
  SDL_Texture* smode; int smupS[8]; int smdownS[8];
  SDL_Texture* smodeactive;
  SDL_Texture* sfilt; int sfiltlS; int sfilthS; int sfiltbS;
  SDL_Texture* sfiltactive;
  SDL_Texture* srange;
  SDL_Texture* srangebutton;
  SDL_Texture* sselect; int sselectS;
  // knobs //
  OTrackKnob* testk;
  OTrackKnob* kVol;
  OTrackKnob* kPan;
  OTrackKnob* kPitch;
  OTrackKnob* kVolAmp;
  OTrackKnob* kVolCap;
  OTrackKnob* kPanAmp;
  OTrackKnob* kPanCap;
  OTrackKnob* kPitchAmp;
  OTrackKnob* kPitchCap;
  OTrackKnob* kCutAmp;
  OTrackKnob* kCutCap;
  OTrackKnob* kResAmp;
  OTrackKnob* kResCap;
  // text fields //
  OTrackText* testt;
  OTrackText* tSName;
  OTrackText* tEName;
  string stri;
  // eye candy //
  float aBBPos;
  int selRot;
  float fc;
  // temporary SDL stuff //
  SDL_Rect tempr, tempr1;
  SDL_Color tempc;
  // internal switches //
  bool showLoad;
  bool showSampleSel;
  bool showEnvSel;
  bool showHidden;
  // busy indicator //
  bool busy;
  bool abusy;
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
  // grid variables //
  int selRegion;
  bool gridGrab;
  int grabWhat;
  // sample edit variables //
  int curSample;
  // envSTarget:
  // 0: envelope
  // 1: volume
  // 2: panning
  // 3: pitch
  // 4: cutoff
  // 5: resonance
  int envSTarget;
  // envelope edit variables //
  int selPoint;
  bool selGrab;
  int curEnv;
  // envpoint menu variables //
  bool pMenuVis;
  int pMenuTarget;
  int pMenuSel;
  SDL_Rect pMenuPos;
  // keypad variables //
  string kpCurVal;
  float* kpVar;
  // interpolation methods //
  float intNone(float* b, int n, float d, float delta);
  float intINearest(float* b, int n, float d, float delta);
  float intLinear(float* b, int n, float d, float delta);
  float intCubic(float* b, int n, float d, float delta);
  float intSinc(float* b, int n, float d, float delta);
  // vector-like functions //
  void pResize(envp** which, size_t* cursize, size_t newsize);
  void pErase(envp** which, size_t* cursize, size_t which1);
  void eResize(size_t newsize);
  void sResize(size_t newsize);
  void vResize(size_t newsize);
  void vErase(size_t which);
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
  void sumMouseDown(int button);
  void sumMouseUp(int button);
  void sumMouseMove(int button);
  void grMouseDown(int button);
  void grMouseUp(int button);
  void grMouseMove(int button);
  void seMouseDown(int button);
  void seMouseUp(int button);
  void seMouseMove(int button);
  void envMouseDown(int button);
  void envMouseUp(int button);
  void envMouseMove(int button);
  // loading functions //
  void initEnv(int which);
  void initSample(int which);
  void loadAction();
  void loadSample();
  // list functions //
  void clearList();
  void feedList(string name, string rh, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
  void drawList();
  void drawLoadUI();
  void prepareSampleSel();
  void prepareEnvSel(bool addNone);
  // draw UI functions //
  void drawSampleSel();
  void drawEnvSel();
  void drawSummary();
  void drawGrid();
  void drawEnvEdit();
  void drawSampleEdit();
  // directory functions //
  string topLevel(string path);
  int readDir(const char* path);
  // update envelopes //
  void updateEnv(envl* envel, int* posN, float* posD, int* envpi, voice* object);
  public:
    OTrackInsSpec* getspec();
    float* getSample();
    unsigned char* saveState(int* size);
    bool loadState(FILE* data);
    void setRenderer(SDL_Renderer* renderer);
    void mouseEvent(int type, int button, int x, int y, int finger);
    void keyEvent(int type, int time, bool repeat, int keycode, int vkeycode, int keymod);
    void textEvent(char* data);
    void drawUI();
    void reset();
    bool init(int inChannels, int outChannels);
};
