#include "sampler.h"

void Sampler::sumMouseMove(int button) {
  kVol->mouseMove(mouse.x,mouse.y);
  kPan->mouseMove(mouse.x,mouse.y);
  kPitch->mouseMove(mouse.x,mouse.y);
}

void Sampler::sumMouseDown(int button) {
  kVol->mouseDown(mouse.x,mouse.y,button);
  kPan->mouseDown(mouse.x,mouse.y,button);
  kPitch->mouseDown(mouse.x,mouse.y,button);
}

void Sampler::sumMouseUp(int button) {
  kVol->mouseUp(mouse.x,mouse.y,button);
  kPan->mouseUp(mouse.x,mouse.y,button);
  kPitch->mouseUp(mouse.x,mouse.y,button);
}

void Sampler::drawSummary() {
  kVol->setPos(10,10);
  kVol->draw();
  kPan->setPos(84,10);
  kPan->draw();
  kPitch->setPos(158,10);
  kPitch->draw();
  //f->draw(42,170,tempc,1,0,0,"Amount");
}
