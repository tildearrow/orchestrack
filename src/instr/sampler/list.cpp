#include "sampler.h"

void Sampler::listMouseMove(int button) {
  if (touching) {
    listPos=-mouse.y+touchSPos;
    scrolling=true;
  }
}

void Sampler::listMouseDown(int button) {
  if (PointInRect(mouse.x,mouse.y,33,63,33+674,63+392)) {
    // swipe code
    touching=true;
    scrolling=false;
    touchSPos=listPos+mouse.y;
    listSpeed=0;
  }
}

void Sampler::listMouseUp(int button) {
  if (touching) {
    touching=false;
    listSpeed=fabs(polledMY-oldPolledMY);
    listDir=(polledMY-oldPolledMY)>0;
    if (listPos<0 || (listPos+382)>20*(listelem.size())) {
      listSpeed=0;
    }
  }
  if (PointInRect(mouse.x,mouse.y,33,63,33+674,63+392)) {
    if (!scrolling) {
      if (loadHIndex==(int)((mouse.y-63+listPos)/20) && loadHIndex<(int)listelem.size()) {
        if (showSampleSel) {
          curSample=loadHIndex;
          showSampleSel=false;
        } else if (showEnvSel) {
          printf("envelope code here.\n");
          curEnv=loadHIndex;
          showEnvSel=false;
        } else {
          loadSample();
        }
      } else {
        loadHIndex=(!PointInRect(mouse.x,mouse.y,30,60,30+680,60+392))?(-1):((mouse.y-63+(int)listPos)/20);
        if (loadHIndex<(int)listelem.size() && loadHIndex>-1) {
          sfname=listelem[loadHIndex].name;
        } else {
          sfname="";
        }
      }
    } else {
      scrolling=false;
    }
  }
}

void Sampler::listMouseWheel(int x, int y) {
  if (y==0) {
    return;
  }
#ifdef __APPLE__
  listSpeed+=fabs((float)y)*2;
#else
  listSpeed+=fabs((float)y)*8;
#endif
  listDir=(y>0)?(1):(0); 
}

void Sampler::clearList() {
  listelem.resize(0);
}

void Sampler::feedList(string name, string rh, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
  listentry tle;
  tle.name=name;
  tle.rh=rh;
  tle.color.r=r;
  tle.color.g=g;
  tle.color.b=b;
  tle.color.a=a;
  listelem.push_back(tle);
}

void Sampler::drawList() {
  tempr.x=30; tempr1.x=0;
  tempr.y=60; tempr1.y=0;
  tempr.w=680;  tempr1.w=680;
  tempr.h=392;  tempr1.h=392;
  SDL_RenderCopy(r,slflist,&tempr1,&tempr);
  
  SDL_Rect clipr;
  clipr.x=32; clipr.y=62; clipr.w=676; clipr.h=388;
  SDL_RenderSetClipRect(r,&clipr);
  if (loadHIndex!=-1 && loadHIndex<(int)listelem.size()) {
    SDL_SetRenderDrawColor(r,255,255,255,(!scrolling && touching && loadHIndex==(int)((mouse.y-63+listPos)/20))?(128):(64));
    tempr.x=33;
    tempr.y=66+20*loadHIndex-(int)listPos;
    tempr.w=674;
    tempr.h=20;
    SDL_RenderFillRect(r,&tempr);
  }

  for (int i=(int)fmax(0,listPos/20); i<fmin(listelem.size(),20+listPos/20); i++) {
    f->draw(33,66+(20*i)-(int)listPos,listelem[i].color,0,0,0,listelem[i].name);
    f->draw(33+671,66+(20*i)-(int)listPos,listelem[i].color,2,0,0,listelem[i].rh);
  }
  
  SDL_RenderSetClipRect(r,NULL);
  
  oldPolledMY=polledMY;
  polledMY=(float)mouse.y;
  
  if (!touching) {
    if (listDir) {
      listPos-=listSpeed;
    } else {
      listPos+=listSpeed;
    }
    if ((listPos+382)>20*(listelem.size()) || listPos<0) {
      listSpeed*=0.6f;
    } else {
      listSpeed*=0.9f;
    }
    if (listSpeed<0.1) {
      listSpeed=0;
    }
  }
  
  if (listPos<0 && !touching) {
    listPos+=-listPos/8;
    if (listPos>-0.5) {
      listPos=0;
    }
  }
  
  if (listPos>0 && (listPos+382)>20*(listelem.size()) && !touching) {
    if (392>20*listelem.size()) {
      listPos+=-listPos/8;
      if (listPos<0.5) {
        listPos=0;
      }
    } else {
      listPos-=((listPos+382)-20*listelem.size())/8;
      if (listPos<((20*(float)listelem.size())+0.5-392)) {
        listPos=(20*(float)listelem.size()-392);
      }
    }
  }
}
