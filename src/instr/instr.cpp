#include "instr.h"

void OTrackInstrument::submitEvent(void* data) {
  event.push(data);
}

void* OTrackInstrument::getEvent() {
  void* popped;
  if (event.size()) {
    popped=event.front();
    event.pop();
    return popped;
  } else {
    return 0;
  }
}

void OTrackInstrument::setRenderer(SDL_Renderer* renderer) {
  r=renderer;
}