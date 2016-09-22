#ifndef FONT_H
#define FONT_H
#include "../includes.h"

class font {
  SDL_Renderer* renderer;
  SDL_Surface* temps;
  SDL_Texture* tempt;
  int* mx;
  int* my;
  char* formatcache;
  int curfsize;
  SDL_Rect tempr;
  public:
  TTF_Font* f;
  void draw(int x, int y, SDL_Color col, int align, int valign, bool nocache, string text);
  void drawf(int x, int y, SDL_Color col, int align, int valign, const char* format, ...);
  int load(const char* filename, int size);
  void setrenderer(SDL_Renderer* r);
  void setcursor(int* mx, int* my);
};
#endif
