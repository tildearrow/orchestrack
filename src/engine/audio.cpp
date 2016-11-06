#include "engine.h"

float* OTrackEngine::getSample() {
  iii++;
  so[0]=(float)(iii%441)/441;
  so[1]=(float)(iii%441)/441;
  return so;
}
