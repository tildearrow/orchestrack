// knob class
#include "../includes.h"

class OTrackKnob {
  float* val;
  float rmin, rmax, rval, hardness;
  float target;
  int xx, yy, w, h;
  bool hover, drag, reset;
  int hoverTime, dragStart;
  float dragVal;
  SDL_Renderer* rend;
  SDL_Texture* tex;
  SDL_Texture* tex1;
  SDL_Texture* tex2;
  SDL_Texture* light;
  public:
    void draw();
    void setRange(float min, float max, float resetval);
    void setOut(float* out);
    void setPos(int x, int y);
    void mouseMove(int x, int y);
    void mouseDown(int x, int y, int button);
    void mouseUp(int x, int y, int button);
    OTrackKnob(SDL_Renderer* renderer, int rad, unsigned char r, unsigned char g, unsigned char b);
};
