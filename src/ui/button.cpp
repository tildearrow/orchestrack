#include "button.h"

int drawButton(SDL_Renderer* r, int x, int y, int w, int h, SDL_Color color, float rr) {
  SDL_Rect rect;
  rect.x=0; rect.y=0; rect.w=w; rect.h=h;
  SDL_Texture* t;
  t=SDL_CreateTexture(r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,w,h);
  void* p=NULL;
  int pitch;
  if (!SDL_LockTexture(t,NULL,&p,&pitch)) {
    unsigned char* pp;
    pp=(unsigned char*)p;
    float res;
    for (int i=0; i<w*h; i++) {
        res=255*(pow(pow(fabs((i%w)-(w/2)),rr)+pow(fabs((i/w)-(h/2)),rr),1.0/rr)<(w/2));
      pp[(i*4)]=res; pp[(i*4)+1]=res; pp[(i*4)+2]=res; pp[(i*4)+3]=255;
    }
    SDL_UnlockTexture(t);
  } else {
    fprintf(stderr,"can't.\n");
  }
  rect.x=x; rect.y=y;
  SDL_RenderCopy(r,t,NULL,&rect);
  SDL_DestroyTexture(t);
}
