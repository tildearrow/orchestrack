#include "sampler.h"

void Sampler::grMouseMove(int button) {
  if (!gridGrab) {
    selRegion=-1;
    for (int i=sSize-1; i>=0; i--) {
      if (PointInRect(mouse.x,mouse.y,
                      48+(s[i].noteMin*5),
                      (int)(28+336.0f*((float)s[i].velMin/127.0f)),
                      56+(5*(s[i].noteMax)),
                      (int)(36+336.0f*((float)(s[i].velMax)/127.0f)))) {
        selRegion=i;
        break;
      }
    }
  } else {
    // move point.
    if (grabWhat==1 || grabWhat==4 || grabWhat==7) {
      s[selRegion].noteMin=fmax(0,fmin(s[selRegion].noteMax,(mouse.x-48)/5));
    }
    if (grabWhat==3 || grabWhat==6 || grabWhat==9) {
      s[selRegion].noteMax=fmax(s[selRegion].noteMin,fmin(127,(mouse.x-48)/5));
    }
    if (grabWhat==1 || grabWhat==2 || grabWhat==3) {
      s[selRegion].velMin=fmax(0,fmin(s[selRegion].velMax,((mouse.y-28)*127)/336));
    }
    if (grabWhat==7 || grabWhat==8 || grabWhat==9) {
      s[selRegion].velMax=fmax(s[selRegion].velMin,fmin(127,((mouse.y-28)*127)/336));
    }
    // TODO: special case for center
  }
}

void Sampler::grMouseDown(int button) {
  if (selRegion!=-1) {
    // check what did we grab
    gridGrab=true;
    SDL_CaptureMouse(SDL_TRUE);
    if (mouse.y>=(int)(28+336.0f*((float)s[selRegion].velMin/127.0f)) &&
        mouse.y<=(int)(36+336.0f*((float)s[selRegion].velMin/127.0f))) {
      // up
      if (mouse.x<56+(s[selRegion].noteMin*5)) {
        printf("upper left\n");
        grabWhat=1;
      } else if (mouse.x>48+(s[selRegion].noteMax*5)) {
        printf("upper right\n");
        grabWhat=3;
      } else {
        printf("up\n");
        grabWhat=2;
      }
    } else if (mouse.y>=(int)(28+336.0f*((float)s[selRegion].velMax/127.0f)) &&
               mouse.y<=(int)(36+336.0f*((float)s[selRegion].velMax/127.0f))) {
      // down
      if (mouse.x<56+(s[selRegion].noteMin*5)) {
        printf("lower left\n");
        grabWhat=7;
      } else if (mouse.x>48+(s[selRegion].noteMax*5)) {
        printf("lower right\n");
        grabWhat=9;
      } else {
        printf("down\n");
        grabWhat=8;
      }
    } else {
      // middle
      if (mouse.x<56+(s[selRegion].noteMin*5)) {
        printf("left\n");
        grabWhat=4;
      } else if (mouse.x>48+(s[selRegion].noteMax*5)) {
        printf("right\n");
        grabWhat=6;
      } else {
        printf("center\n");
        grabWhat=0;
      }
    }
    printf("grid grab.\n");
  }
}

void Sampler::grMouseUp(int button) {
  if (gridGrab) {
    gridGrab=false;
    SDL_CaptureMouse(SDL_FALSE);
    printf("grid ungrab.\n");
  }
}

void Sampler::drawGrid() {
  tempr.x=50;  tempr1.x=0;
  tempr.y=30;  tempr1.y=0;
  tempr.w=639; tempr1.w=639;
  tempr.h=340; tempr1.h=340;
  SDL_RenderCopy(r,grid,&tempr1,&tempr);
  tempr.x=0;
  tempr.y=0;
  tempr.w=128;
  tempr.h=64;
  // draw currently playing notes
  SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_ADD);
  SDL_SetRenderDrawColor(r,255,255,255,32);
  for (size_t i=0; i<vSize; i++) {
    tempr.x=v[i].note*5;
    tempr.y=10;
    tempr.w=5;
    tempr.h=340;
    SDL_RenderFillRect(r,&tempr);
  }
  SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
  // draw sample regions
  for (size_t i=0; i<sSize; i++) {
    SDL_SetRenderDrawColor(r,40,128,255,(i==selRegion && gridGrab)?(72):(48));
    tempr.x=50+2+s[i].noteMin*5;
    tempr.y=(int)(32+336.0f*((float)s[i].velMin/127.0f));
    tempr.w=5*(s[i].noteMax-s[i].noteMin);
    tempr.h=(int)(336.0f*((float)(s[i].velMax-s[i].velMin)/127.0f));
    SDL_RenderFillRect(r,&tempr);
    SDL_RenderDrawRect(r,&tempr);
  }
  // if hovering over region, draw grab points
  if (selRegion!=-1) {
    SDL_SetRenderDrawColor(r,128,192,255,255);
    tempr.w=8;
    tempr.h=8;
    tempr.x=50+2+s[selRegion].noteMin*5-4;
    tempr.y=(int)(32+336.0f*((float)s[selRegion].velMin/127.0f))-4;
    SDL_RenderDrawRect(r,&tempr);
    tempr.x=50+2+s[selRegion].noteMax*5-4;
    tempr.y=(int)(32+336.0f*((float)s[selRegion].velMin/127.0f))-4;
    SDL_RenderDrawRect(r,&tempr);
    tempr.x=50+2+s[selRegion].noteMin*5-4;
    tempr.y=(int)(32+336.0f*((float)s[selRegion].velMax/127.0f))-4;
    SDL_RenderDrawRect(r,&tempr);
    tempr.x=50+2+s[selRegion].noteMax*5-4;
    tempr.y=(int)(32+336.0f*((float)s[selRegion].velMax/127.0f))-4;
    SDL_RenderDrawRect(r,&tempr);
    f->drawf(52+((s[selRegion].noteMin+s[selRegion].noteMax)/2)*5
             ,(int)(32+336.0f*((float)s[selRegion].velMin/127.0f))
             ,tempc,1,2,"%d",s[selRegion].velMin);
    f->drawf(52+((s[selRegion].noteMin+s[selRegion].noteMax)/2)*5
             ,(int)(32+336.0f*((float)s[selRegion].velMax/127.0f))
             ,tempc,1,0,"%d",s[selRegion].velMax);
    f->drawf(52+(s[selRegion].noteMin*5)
             ,(int)(32+336.0f*((float)(s[selRegion].velMin+s[selRegion].velMax)/255.0f))
             ,tempc,2,1,"%c%c%d",sChromaNote[s[selRegion].noteMin%12]
                                ,sChromaSemitone[s[selRegion].noteMin%12]
                                ,(s[selRegion].noteMin/12)-2);
    f->drawf(52+(s[selRegion].noteMax*5)
             ,(int)(32+336.0f*((float)(s[selRegion].velMin+s[selRegion].velMax)/255.0f))
             ,tempc,0,1,"%c%c%d",sChromaNote[s[selRegion].noteMax%12]
                                ,sChromaSemitone[s[selRegion].noteMax%12]
                                ,(s[selRegion].noteMax/12)-2);
    f->draw(52+((s[selRegion].noteMin+s[selRegion].noteMax)/2)*5
             ,(int)(32+336.0f*((float)(s[selRegion].velMin+s[selRegion].velMax)/255.0f))
             ,tempc,1,1,0,s[selRegion].path[0]);
  } else {
    f->draw(370,12,tempc,1,0,0,"Note");
    f->draw(48,200,tempc,2,1,0,"Vol");
  }
  
  //f->drawf(83,400,tempc,0,0,"%d %d %d %d",tick/96,(tick/24)%4,(tick/6)%4,tick%6);
}
