#include "loader.h"

class OTrackMODImport: public OTrackLoader {
  OTrackProject* getProject(FILE* f);
};
