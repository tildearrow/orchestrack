#include "../includes.h"
#include "../instr/sampler/sampler.h"
#include "../engine/project.h"

class OTrackApp {
  SDL_Window* w;
  SDL_Renderer* r;
  SDL_Event e;
  bool quit, windowDrag;
  int count;
  int selWindow, wDx, wDy;
  OTrackProject* p;
  SDL_Texture* testt;
  SDL_Texture* testt1;
  SDL_Rect bound;
  // mouse structure //
  struct {
    int x;
    int y;
    bool b[4];
  } mouse;
  // drawing functions //
  void drawUI();
  void mouseMove();
  void mouseDown(int button);
  void mouseUp(int button);
  public:
    int init();
    int loop();
};
