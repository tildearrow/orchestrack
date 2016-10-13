#include "../includes.h"
#include "application.h"

OTrackApp* app;

int main(int argc, char** argv) {
  app=new OTrackApp;
  app->init();
  app->loop();
  printf("\n");
  return 0;
}

#ifdef __ANDROID__
int SDL_main(int argc, char** argv) {
  main(0,NULL);
}
#endif
