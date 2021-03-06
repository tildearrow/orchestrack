#include "../includes.h"
#ifndef _INSTR_H
#define _INSTR_H
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
  protected:
  SDL_Renderer* r;
  float* sample;
  int inCh, outCh;
  std::queue<void*> event;
  public:
    virtual OTrackInsSpec* getspec();
    std::vector<float> param;
    void submitEvent(void* data);
    void* getEvent();
    virtual float* getSample();
    virtual unsigned char* saveState(int* size);
    virtual bool loadState(FILE* data);
    virtual void drawUI();
    virtual void setRenderer(SDL_Renderer* renderer);
    virtual void mouseEvent(int type, int button, int x, int y, int finger);
    virtual void keyEvent(int type, int time, bool repeat, int keycode, int vkeycode, int keymod);
    virtual void textEvent(char* data);
    virtual bool init(int inChannels, int outChannels);
};
#endif
