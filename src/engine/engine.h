#include "../includes.h"
#include "project.h"

// orchestrack engine class.
// for an easier to use interface, see liborchestra.
// (but not yet)
class OTrackEngine {
  std::queue<void*>* midi;
  public:
    OTrackProject p;
    // Initialize the engine. Should be called
    // before doing anything else.
    int init();
    // Set the sample rate.
    int setRate(int rate);
    // Add an input/output channel to the engine.
    int addChannel(bool input, bool midi);
    // Retrieve a pointer to next audio sample,
    // ordered by channels. A MIDI channel will
    // be represented with NaN.
    float* getSample();
    // Upload a sample to the engine.
    // Returns whether operation was successful.
    int putSample(float* sample);
    // Get/put a MIDI event.
    // Returns NULL or -1 on failure.
    unsigned char* getMEvent(int channel);
    int putMEvent(int channel, void* data);
};
