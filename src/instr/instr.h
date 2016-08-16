#include "../../includes.h"

class OTrackInstrument {
  SDL_Renderer* r;
  std::queue<void*> event;
  public:
    std::vector<float> param;
    void submitEvent(void* data);
    void* getEvent();
    virtual void getSample(int channels, float* sample, ...);
    virtual void drawUI();
    void setRenderer(SDL_Renderer* renderer);
};