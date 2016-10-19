#include "sampler.h"

void Sampler::keyEvent(int type, int time, bool repeat, int keycode, int vkeycode, int keymod) {
  testt->keyEvent(keycode,vkeycode,repeat,type);
}

void Sampler::textEvent(char* data) {
  testt->keyInputEvent(data);
}
