#include "driver.h"
#include <jack/jack.h>
#include <jack/midiport.h>

class OTrackJACKDriver: public OTrackADriver {
  jack_client_t* c;
  jack_status_t s;
  public:
    int init();
}
