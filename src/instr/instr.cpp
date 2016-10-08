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

bool OTrackInstrument::loadState(unsigned char* data, int size) {
  return false;
}

unsigned char* OTrackInstrument::saveState(int* size) {
  return NULL;
}

// Please modify when making instruments.
OTrackInsSpec* OTrackInstrument::getspec() {
  OTrackInsSpec* aspec;
  aspec=new OTrackInsSpec;
  aspec->name="NullInstrument";
  aspec->desc="null instrument";
  aspec->author="nobody";
  aspec->copyright="none";
  aspec->license="none";
  return aspec;
}

void OTrackInstrument::setRenderer(SDL_Renderer* renderer) {
  r=renderer;
}

float* OTrackInstrument::getSample() {
  for (int i=0; i<outCh; i++) {
    sample[i]=0;
  }
  return sample;
}

void OTrackInstrument::drawUI() {
}

void OTrackInstrument::mouseEvent(int type, int button, int x, int y, int finger) {
}

bool OTrackInstrument::init(int inChannels, int outChannels) {
  if (inChannels==0) {
    sample=new float[outChannels];
    outCh=outChannels;
    return true;
  } else {
    return false;
  }
}
