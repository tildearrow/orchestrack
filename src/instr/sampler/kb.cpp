#include "sampler.h"

void Sampler::keyEvent(int type, int time, bool repeat, int keycode, int vkeycode, int keymod) {
  switch (curView) {
    case 2:
      tSName->keyEvent(keycode,vkeycode,repeat,type);
      break;
    case 3:
      tEName->keyEvent(keycode,vkeycode,repeat,type);
      break;
  }
}

void Sampler::textEvent(char* data) {
  switch (curView) {
    case 2:
      tSName->keyInputEvent(data);
      break;
    case 3:
      tEName->keyInputEvent(data);
      break;
  }
}
