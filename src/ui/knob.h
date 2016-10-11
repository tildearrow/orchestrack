// knob class
#include "../includes.h"

class OTrackKnob {
  float* val;
  float rmin, rmax;
  float target;
  int w, h;
  SDL_Renderer* rend;
  SDL_Texture* tex;
  SDL_Texture* tex1;
  SDL_Texture* light;
  public:
    void draw(int x, int y);
    void setRange(float min, float max);
    void setOut(float* out);
    void mouseMove(int x, int y);
    void mouseDown(int x, int y, int button);
    void mouseUp(int x, int y, int button);
    OTrackKnob(SDL_Renderer* renderer, int rad, unsigned char r, unsigned char g, unsigned char b);
};
