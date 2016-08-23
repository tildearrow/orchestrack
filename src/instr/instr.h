#include "../includes.h"

// Instrument specification.
struct OTrackInsSpec {
  const char* name;
  const char* desc;
  const char* author;
  const char* copyright;
  const char* license;
  int inChannels, outChannels;
  bool midiIn, midiOut;
};

class OTrackInstrument {
  SDL_Renderer* r;
  float* sample;
  std::queue<void*> event;
  public:
    virtual OTrackInsSpec* getspec();
    std::vector<float> param;
    void submitEvent(void* data);
    void* getEvent();
    virtual void* getSample();
    virtual void drawUI();
    void setRenderer(SDL_Renderer* renderer);
    void init(OTrackInsSpec* initSpec, OTrackInsSpec* resultSpec);
};
