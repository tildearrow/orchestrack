#include "../includes.h"
#include "../instr/sampler/sampler.h"
#include "../engine/project.h"

class OTrackApp {
  SDL_Window* w;
  SDL_Renderer* r;
  SDL_Event e;
  bool quit;
  int count;
  OTrackProject* p;
  OTrackInstrument* testi;
  OTrackInstrument* testi1;
  SDL_Texture* testt;
  SDL_Texture* testt1;
  SDL_Rect bound, bound1;
  public:
    int init();
    int loop();
};
