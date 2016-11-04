#include "../includes.h"
#include "project.h"

// orchestrack engine class.
// for an easier to use interface, see liborchestra.
// (but not yet)
class OTrackEngine {
  std::queue<void*>* mi;
  std::queue<void*>* mo;
  float* si;
  float* so;
  int sichans, sochans;
  int michans, mochans;
  public:
    OTrackProject p;
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
