#include "font.h"

int font::load(const char* filename, int size) {
  f=TTF_OpenFont(filename, size);
  if (f==NULL) {return 0;} else {return 1;}
  curfsize=64;
  formatcache=new char[curfsize];
}

void font::setrenderer(SDL_Renderer* r) {
  renderer=r;
}

void font::drawf(int x, int y, SDL_Color col, int align, int valign, const char* format, ...) {
  if (strcmp(format,"")==0) {return;}
  int chars;
  chars=0;
  va_list va;
  va_start(va,format);
  char* fs;
  #ifdef _WIN32
  // temporary workaround...
  fs=new char[2048];
  vsnprintf(fs,2047,format,va);
  #else
  vasprintf(&fs,format,va);
  #endif
  va_end(va);
  string tempstring;
  tempstring=fs;
  delete[] fs;
  draw(x,y,col,align,valign,1,tempstring);
}

void font::draw(int x, int y, SDL_Color col, int align, int valign, bool nocache, string text) {
  if (text=="") {return;}
  temps=TTF_RenderUTF8_Blended(f, text.c_str(), col);
  if (temps==NULL) {printf("aaaa\n");return;}
  tempt=SDL_CreateTextureFromSurface(renderer, temps);
  tempr.x=x-((align)?(temps->clip_rect.w/(3-align)):(0));
  tempr.y=y-((valign)?(temps->clip_rect.h/(3-valign)):(0));
  tempr.w=temps->clip_rect.w;
  tempr.h=temps->clip_rect.h;
  SDL_SetTextureAlphaMod(tempt,col.a);
  SDL_RenderCopy(renderer, tempt, &temps->clip_rect, &tempr);
  SDL_DestroyTexture(tempt);
  SDL_FreeSurface(temps);
}
