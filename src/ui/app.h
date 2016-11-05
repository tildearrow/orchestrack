#include "../includes.h"
#include <jack/jack.h>
#include <jack/midiport.h>
#include "../font/font.h"
#include "../instr/sampler/sampler.h"
#include "../engine/engine.h"

class OTrackApp {
  SDL_Window* w;
  SDL_Renderer* r;
  SDL_Event e;
  SDL_Color tempc;
  font* f;
  bool quit, windowDrag;
  int count;
  int selWindow, wDx, wDy;
  int dw, dh;
  int curView;
  OTrackEngine* p;
  SDL_Texture* testt;
  SDL_Texture* testt1;
  SDL_Rect bound;
  // audio. //
  // for now, JACK.
  // portaudio coming soon.
  jack_client_t* ac;
  jack_status_t as;
  jack_port_t** ao; // audio
  jack_port_t** mo; // MIDI
  // mouse structure //
  struct {
    int x;
    int y;
    bool b[4];
  } mouse;
  // top bar textures //
  SDL_Texture* bVPattern; int bVPatternS;
  SDL_Texture* bVWorkspace; int bVWorkspaceS;
  SDL_Texture* bVPatchbay; int bVPatchbayS;
  // rectangle/color making functions //
  SDL_Rect mR(int x, int y, int w, int h);
  SDL_Color mC(int r, int g, int b, int a);
  // audio //
  static int audio(jack_nframes_t len, void* arg);
  // drawing functions //
  void drawTopBar();
  void drawWorkspace();
  void drawPatchbay();
  void drawPattern();
  void drawUI();
  void mouseMove();
  void mouseDown(int button);
  void mouseUp(int button);
  public:
    int init();
    int loop();
};
