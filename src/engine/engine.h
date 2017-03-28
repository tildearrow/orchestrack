#include "../includes.h"
#include "project.h"

// orchestrack engine class.
// for an easier to use interface, see liborchestra.
// (but not yet)
class OTrackEngine {
  // this will be removed later...
  const char testnote[64]={
    0x3f,0x00,0x00,0x3f,0x3e,0x00,0x3c,0x00,0x43,0x3e,0x3b,0x3e,0x37,0x00,0x3f,0x41,0x43,0x00,0x00,0x43,0x41,0x00,0x3f,0x00,0x46,0x41,0x3e,0x41,0x3a,0x00,
    0x3c,0x3e,0x3f,0x00,0x3f,0x00,0x3f,0x3e,0x3c,0x00,0x3e,0x00,0x3e,0x00,0x3e,0x3f,0x3e,0x3b,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x3c,0x3f,0x44,0x3a,0x3e,0x41,0x46
  };
  int cn;
  int iii;
  std::queue<void*>* mi;
  std::queue<void*>* mo;
  unsigned char** me;
  float* si;
  float* so;
  int sichans, sochans;
  int michans, mochans;
  void updatePos();
  std::queue<void*>* pmo;
  public:
    // syncmode:
    // - 0: audio buffer
    // - 1: vblank
    // - 2: timer
    int syncmode;
    float tempo;
    int speed;
    int pat;
    int row;
    int tick;
    double subtick;
    int mult;
    int play;
    OTrackProject* p;
    // Initialize the engine. Should be called
    // before doing anything else.
    int init();
    // Set the sample rate.
    int setRate(int rate);
    // Add an input/output audio channel to the engine.
    int addChannel(bool input);
    // Add an input/output MIDI channel to the engine.
    int addMChannel(bool input);

    // NOTE ABOUT GET/PUT FUNCTIONS //
    // Your program shall put MIDI events first (if any), followed by putting
    // samples. Then you can do getSample. This is because all the main audio
    // code happens in getSample.

    // Retrieve a pointer to next audio sample,
    // ordered by channels.
    float* getSample();
    // Upload a sample to the engine.
    // Returns whether operation was successful.
    int putSample(float* sample);
    // Get/put a MIDI event.
    // Returns NULL or -1 on failure.
    unsigned char* getMEvent(int channel);
    int putMEvent(int channel, void* data);
};
