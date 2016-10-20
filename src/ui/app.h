#include "../includes.h"

class OTrackApp {
  SDL_Window* w;
  SDL_Renderer* r;
  SDL_Event e;
  bool quit;
  public:
    int init();
    int loop();
};
