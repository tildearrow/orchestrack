#include "sampler.h"

void Sampler::grMouseMove(int button) {
  hover(72-52,124+256,72-52+40,124+24+256,&smupS[0]);
  hover(72-52,192+256,72-52+40,192+24+256,&smdownS[0]);
  hover(72+52,124+256,72+52+40,124+24+256,&smupS[1]);
  hover(72+52,192+256,72+52+40,192+24+256,&smdownS[1]);
  hover(257-52,124+256,257-52+40,124+24+256,&smupS[2]);
  hover(257-52,192+256,257-52+40,192+24+256,&smdownS[2]);
  hover(257+52,124+256,257+52+40,124+24+256,&smupS[3]);
  hover(257+52,192+256,257+52+40,192+24+256,&smdownS[3]);
}

void Sampler::grMouseDown(int button) {
  /// target ///
  
  // note min
  if (PointInRect(mouse.x,mouse.y,72-52,124+256,72-52+40,124+24+256)) {
    smupS[0]=2;
    if (button!=1) {
      doUp=true;
      doXTarget=1;
    } else {
      s[curSample].noteMin+=12;
    }
  }
  if (PointInRect(mouse.x,mouse.y,72-52,192+256,72-52+40,192+24+256)) {
    smdownS[0]=2;
    if (button!=1) {
      doDown=true;
      doXTarget=1;
    } else {
      s[curSample].noteMin-=12;
    }
  }
  
  // note max
  if (PointInRect(mouse.x,mouse.y,72+52,124+256,72+52+40,124+24+256)) {
    smupS[1]=2;
    if (button!=1) {
      doUp=true;
      doXTarget=2;
    } else {
      s[curSample].noteMax+=12;
    }
  }
  if (PointInRect(mouse.x,mouse.y,72+52,192+256,72+52+40,192+24+256)) {
    smdownS[1]=2;
    if (button!=1) {
      doDown=true;
      doXTarget=2;
    } else {
      s[curSample].noteMax-=12;
    }
  }
  
  // vol min
  if (PointInRect(mouse.x,mouse.y,257-52,124+256,257-52+40,124+24+256)) {
    smupS[2]=2;
    if (button!=1) {
      doUp=true;
      doXTarget=3;
    } else {
      s[curSample].velMin*=2;
    }
  }
  if (PointInRect(mouse.x,mouse.y,257-52,192+256,257-52+40,192+24+256)) {
    smdownS[2]=2;
    if (button!=1) {
      doDown=true;
      doXTarget=3;
    } else {
      s[curSample].velMin/=2;
    }
  }
  
  // vol max
  if (PointInRect(mouse.x,mouse.y,257+52,124+256,257+52+40,124+24+256)) {
    smupS[3]=2;
    if (button!=1) {
      doUp=true;
      doXTarget=4;
    } else {
      s[curSample].velMax*=2;
    }
  }
  if (PointInRect(mouse.x,mouse.y,257+52,192+256,257+52+40,192+24+256)) {
    smdownS[3]=2;
    if (button!=1) {
      doDown=true;
      doXTarget=4;
    } else {
      s[curSample].velMax/=2;
    }
  }

}

void Sampler::grMouseUp(int button) {
  /// range ///
  
  if (smupS[0]!=1) {
    smupS[0]=PointInRect(mouse.x,mouse.y,72-52,124+256,72-52+40,124+24+256);
    doUp=false;
  }
  if (smdownS[0]!=1) {
    smdownS[0]=PointInRect(mouse.x,mouse.y,72-52,192+256,72-52+40,192+24+256);
    doDown=false;
  }
  
  if (smupS[1]!=1) {
    smupS[1]=PointInRect(mouse.x,mouse.y,72+52,124+256,72+52+40,124+24+256);
    doUp=false;
  }
  if (smdownS[1]!=1) {
    smdownS[1]=PointInRect(mouse.x,mouse.y,72+52,192+256,72+52+40,192+24+256);
    doDown=false;
  }
  
  if (smupS[2]!=1) {
    smupS[2]=PointInRect(mouse.x,mouse.y,257-52,124+256,257-52+40,124+24+256);
    doUp=false;
  }
  if (smdownS[2]!=1) {
    smdownS[2]=PointInRect(mouse.x,mouse.y,257-52,192+256,257-52+40,192+24+256);
    doDown=false;
  }
  
  if (smupS[3]!=1) {
    smupS[3]=PointInRect(mouse.x,mouse.y,257+52,124+256,257+52+40,124+24+256);
    doUp=false;
  }
  if (smdownS[3]!=1) {
    smdownS[3]=PointInRect(mouse.x,mouse.y,257+52,192+256,257+52+40,192+24+256);
    doDown=false;
  }
}

void Sampler::drawGrid() {
  tempr.x=10;  tempr1.x=0;
  tempr.y=10;  tempr1.y=0;
  tempr.w=720; tempr1.w=720;
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
  SDL_SetRenderDrawColor(r,64,255,40,64);
  // draw sample regions
  for (size_t i=0; i<sSize; i++) {
    tempr.x=10+2+s[i].noteMin*5;
    tempr.y=(int)(12+336.0f*((float)s[i].velMin/127.0f));
    tempr.w=5*(s[i].noteMax-s[i].noteMin);
    tempr.h=(int)(336.0f*((float)(s[i].velMax-s[i].velMin)/127.0f));
    SDL_RenderFillRect(r,&tempr);
    SDL_RenderDrawRect(r,&tempr);
  }
  f->draw(370,16,tempc,1,0,0,"Note");
  f->draw(0,200,tempc,1,1,0,"NoteVol");
  f->drawf(83,400,tempc,0,0,"%d %d %d %d",tick/96,(tick/24)%4,(tick/6)%4,tick%6);
  
  f->draw(370,70+256,tempc,1,0,0,"Range");
  f->draw(92,100+256,tempc,1,0,0,"Note");
  f->draw(277,100+256,tempc,1,0,0,"NoteVol");
  f->draw(462,100+256,tempc,1,0,0,"Param1");
  f->draw(647,100+256,tempc,1,0,0,"Param2");
  f->draw(92,170+256,tempc,1,1,0,"to");
  f->draw(277,170+256,tempc,1,1,0,"to");
  f->draw(462,170+256,tempc,1,1,0,"to");
  f->draw(647,170+256,tempc,1,1,0,"to");
  
  /// note
  
  // left
  tempr.x=62-52; tempr1.x=0;
  tempr.y=152+256; tempr1.y=0;
  tempr.w=60;  tempr1.w=60;
  tempr.h=36;  tempr1.h=36;
  SDL_RenderCopy(r,srange,&tempr1,&tempr);
  tempr.x=72-52; tempr1.x=40*smupS[0];
  tempr.y=124+256; tempr1.y=0;
  tempr.w=40;  tempr1.w=40;
  tempr.h=24;  tempr1.h=24;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  tempr.x=72-52; tempr1.x=40*smdownS[0];
  tempr.y=192+256; tempr1.y=0;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  f->draw(92-52,126+256,tempc,1,0,0,"Up");
  f->draw(92-52,194+256,tempc,1,0,0,"Down");
  f->drawf(92-52,170+256,tempc,1,1,
           "%c%c%d",sChromaNote[s[curSample].noteMin%12]
                   ,sChromaSemitone[s[curSample].noteMin%12]
                   ,(s[curSample].noteMin/12)-2);
  
  // right
  tempr.x=62+52; tempr1.x=0;
  tempr.y=152+256; tempr1.y=0;
  tempr.w=60;  tempr1.w=60;
  tempr.h=36;  tempr1.h=36;
  SDL_RenderCopy(r,srange,&tempr1,&tempr);
  tempr.x=72+52; tempr1.x=40*smupS[1];
  tempr.y=124+256; tempr1.y=0;
  tempr.w=40;  tempr1.w=40;
  tempr.h=24;  tempr1.h=24;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  tempr.x=72+52; tempr1.x=40*smdownS[1];
  tempr.y=192+256; tempr1.y=0;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  f->draw(92+52,126+256,tempc,1,0,0,"Up");
  f->draw(92+52,194+256,tempc,1,0,0,"Down");
  f->drawf(92+52,170+256,tempc,1,1,
           "%c%c%d",sChromaNote[s[curSample].noteMax%12]
                   ,sChromaSemitone[s[curSample].noteMax%12]
                   ,(s[curSample].noteMax/12)-2);
  
  /// notevol
  
  // left
  tempr.x=247-52; tempr1.x=0;
  tempr.y=152+256; tempr1.y=0;
  tempr.w=60;  tempr1.w=60;
  tempr.h=36;  tempr1.h=36;
  SDL_RenderCopy(r,srange,&tempr1,&tempr);
  tempr.x=257-52; tempr1.x=40*smupS[2];
  tempr.y=124+256; tempr1.y=0;
  tempr.w=40;  tempr1.w=40;
  tempr.h=24;  tempr1.h=24;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  tempr.x=257-52; tempr1.x=40*smdownS[2];
  tempr.y=192+256; tempr1.y=0;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  f->draw(277-52,126+256,tempc,1,0,0,"Up");
  f->draw(277-52,194+256,tempc,1,0,0,"Down");
  f->drawf(277-52,170+256,tempc,1,1,
           "%d",s[curSample].velMin);
  
  // right
  tempr.x=247+52; tempr1.x=0;
  tempr.y=152+256; tempr1.y=0;
  tempr.w=60;  tempr1.w=60;
  tempr.h=36;  tempr1.h=36;
  SDL_RenderCopy(r,srange,&tempr1,&tempr);
  tempr.x=257+52; tempr1.x=40*smupS[3];
  tempr.y=124+256; tempr1.y=0;
  tempr.w=40;  tempr1.w=40;
  tempr.h=24;  tempr1.h=24;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  tempr.x=257+52; tempr1.x=40*smdownS[3];
  tempr.y=192+256; tempr1.y=0;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  f->draw(277+52,126+256,tempc,1,0,0,"Up");
  f->draw(277+52,194+256,tempc,1,0,0,"Down");
  f->drawf(277+52,170+256,tempc,1,1,
           "%d",s[curSample].velMax);

  upDown();
}
