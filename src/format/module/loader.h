#include "../../includes.h"
#include "../../engine/project.h"

class OTrackLoader {
  virtual OTrackProject* getProject(FILE* f);
};
