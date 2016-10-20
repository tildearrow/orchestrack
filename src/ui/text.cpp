#include "text.h"

void OTrackText::mouseMove(int x, int y) {
  if (PointInRect(x,y,pos.x,pos.y,pos.x+pos.w,pos.y+pos.h)) {
    hover=true;
  } else {
    hover=false;
  }
}

void OTrackText::mouseDown(int x, int y, int button) {
  int sx, sy;
  if (PointInRect(x,y,pos.x,pos.y,pos.x+pos.w,pos.y+pos.h)) {
    input=true;
    SDL_StartTextInput();
    // set cursor to mouse position
    for (int i=val->size(); i>=0; i--) {
      TTF_SizeUTF8(f->f,val->substr(0,i).c_str(),&sx,&sy);
      if (sx<x-pos.x) {
        curpos=i;
        break;
      }
    }
    blink=0;
  } else {
    input=false;
    SDL_StopTextInput();
  }
}

void OTrackText::mouseUp(int x, int y, int button) {
}

void OTrackText::keyEvent(int keycode, int vkeycode, int repeat, int stat) {
  if (stat && input) {
    switch (vkeycode) {
      case SDLK_BACKSPACE:
        if (val->size()>0) {
          val->erase(curpos-1,1);
          curpos--;
        }
        blink=0;
        break;
      case SDLK_LEFT:
        printf("left\n");
        if (curpos>0) {
          curpos--;
        }
        blink=0;
        break;
      case SDLK_RIGHT:
        printf("right\n");
        curpos++;
        if (curpos>val->size()) {
          curpos=val->size();
        }
        blink=0;
        break;
      case SDLK_HOME:
        curpos=0;
        blink=0;
        break;
      case SDLK_END:
        curpos=val->size();
        blink=0;
        break;
    }
  }
}

void OTrackText::keyInputEvent(char* code) {
  if (input) {
    val->insert(curpos,code);
    curpos++;
    blink=0;
  }
}

void OTrackText::setOut(string* out) {
  val=out;
}

void OTrackText::setPos(int x, int y) {
  pos.x=x;
  pos.y=y;
}

void OTrackText::draw() {
  int sx, sy;
  fcolor.r=255;
  fcolor.g=255;
  fcolor.b=255;
  fcolor.a=255;
  SDL_SetTextureColorMod(tex,(hover)?(255):(192),(hover)?(255):(192),(hover)?(255):(192));
  spos.x=pos.w*2*input;
  SDL_RenderCopy(rend,tex,&spos,&pos);
  f->draw(pos.x+3,pos.y,fcolor,0,0,0,val[0]);
  TTF_SizeUTF8(f->f,val->substr(0,curpos).c_str(),&sx,&sy);
  if (input) {
    SDL_SetRenderDrawColor(rend,255,255,255,(blink<30)?(255):(0));
    SDL_RenderDrawLine(rend,pos.x+3+sx,pos.y+3,pos.x+3+sx,pos.y+pos.h-4);
    blink++;
    blink=blink%60;
  }
}

OTrackText::OTrackText(SDL_Renderer* renderer, font* fo, int w, int h, unsigned char r, unsigned char g, unsigned char b) {
  SDL_Color tctc;
  tctc.r=r;
  tctc.g=g;
  tctc.b=b;
  tctc.a=255;
  rend=renderer;
  tex=drawButton(rend,0,0,w,h,tctc,4);
  f=fo;
  pos.w=w; spos.w=w;
  pos.h=h; spos.h=h;
  spos.x=0; spos.y=0;
  curpos=0;
  input=false;
  hover=false;
}
