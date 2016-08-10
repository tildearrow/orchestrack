#include "../includes.h"
#include "application.h"

OTrackApp* app;

int main() {
  app=new OTrackApp;
  app->init();
  app->loop();
  printf("\n");
  return 0;
}
