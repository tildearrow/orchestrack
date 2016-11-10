#include "sampler.h"

void Sampler::drawSummary() {
  kVol->setPos(10,10);
  kVol->draw();
  kPan->setPos(84,10);
  kPan->draw();
  kPitch->setPos(158,10);
  kPitch->draw();
  //f->draw(42,170,tempc,1,0,0,"Amount");
}
