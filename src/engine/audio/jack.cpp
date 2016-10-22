#include "jack.h"

int OTrackJACKDriver::init() {
  c=jack_client_open("otrack",JackNoStartServer,&s,NULL);
  if (c==NULL) {
    return 0;
  }
}
