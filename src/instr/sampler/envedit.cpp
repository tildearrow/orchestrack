#include "sampler.h"

void Sampler::envMouseMove(int button) {
  if (selGrab) {
    e[0].p[selPoint].value=fmin(1,fmax(0,(340.0f-(float)mouse.y)/300.0f));
    if (selPoint!=0) {
      if (selPoint==e[0].pSize-1) {
        e[0].p[selPoint].time=fmax(e[0].p[selPoint-1].time+1,(mouse.x-10)*256);
      } else {
        e[0].p[selPoint].time=fmax(e[0].p[selPoint-1].time+1,fmin((mouse.x-10)*256,e[0].p[selPoint+1].time-1));
      }
    }
  } else {
    selPoint=-1;
    for (size_t i=0; i<e[0].pSize; i++) {
      if (PointInRect(mouse.x,mouse.y,10+(e[0].p[i].time/256)-6,340-(e[0].p[i].value*300.0f)-6,10+(e[0].p[i].time/256)+6,340-(e[0].p[i].value*300.0f)+6)) {
        selPoint=i; break;
      }
    }
    if (pMenuVis) {
      pMenuSel=-1;
      if (PointInRect(mouse.x,mouse.y,pMenuPos.x,pMenuPos.y,pMenuPos.x+106,pMenuPos.y+21)) {
        pMenuSel=0;
      }
      if (PointInRect(mouse.x,mouse.y,pMenuPos.x,pMenuPos.y+28,pMenuPos.x+106,pMenuPos.y+49)) {
        pMenuSel=1;
      }
      if (PointInRect(mouse.x,mouse.y,pMenuPos.x,pMenuPos.y+52,pMenuPos.x+106,pMenuPos.y+71)) {
        pMenuSel=2;
      }
      if (PointInRect(mouse.x,mouse.y,pMenuPos.x,pMenuPos.y+72,pMenuPos.x+106,pMenuPos.y+91)) {
        pMenuSel=3;
      }
      if (PointInRect(mouse.x,mouse.y,pMenuPos.x,pMenuPos.y+92,pMenuPos.x+106,pMenuPos.y+111)) {
        pMenuSel=4;
      }
      if (PointInRect(mouse.x,mouse.y,pMenuPos.x,pMenuPos.y+112,pMenuPos.x+106,pMenuPos.y+131)) {
        pMenuSel=5;
      }
    }
  }
}

void Sampler::envMouseDown(int button) {
  if (button==0) {
    if (pMenuVis) {
      switch (pMenuSel) {
        case 0:
          if (pMenuTarget!=0) {
            // slide loop points
            if (e[0].susStart>=pMenuTarget) {
              e[0].susStart--;
            }
            if (e[0].susEnd>=pMenuTarget) {
              e[0].susEnd--;
            }
            if (e[0].loopStart>=pMenuTarget) {
              e[0].loopStart--;
            }
            if (e[0].loopEnd>=pMenuTarget) {
              e[0].loopEnd--;
            }
            pErase(&e[0].p,&e[0].pSize,pMenuTarget);
          }
          break;
        case 1:
          if (e[0].susStart!=pMenuTarget || e[0].susEnd!=pMenuTarget) {
            e[0].susStart=pMenuTarget;
            e[0].susEnd=pMenuTarget;
          } else {
            e[0].susStart=-1;
            e[0].susEnd=-1;
          }
          break;
        case 2:
          if (e[0].susStart!=pMenuTarget) {
            e[0].susStart=pMenuTarget;
          } else {
            e[0].susStart=-1;
          }
          break;
        case 3:
          if (e[0].susEnd!=pMenuTarget) {
            e[0].susEnd=pMenuTarget;
          } else {
            e[0].susEnd=-1;
          }
          break;
        case 4:
          if (e[0].loopStart!=pMenuTarget) {
            e[0].loopStart=pMenuTarget;
          } else {
            e[0].loopStart=-1;
          }
          break;
        case 5:
          if (e[0].loopEnd!=pMenuTarget) {
            e[0].loopEnd=pMenuTarget;
          } else {
            e[0].loopEnd=-1;
          }
          break;
      }
      pMenuTarget=-1;
      pMenuVis=false;
    } else {
      if (selPoint!=-1) {
        printf("grabbing!\n");
        selGrab=true;
      }
    }
  }
  if (button==2) {
    if (selPoint==-1) {
      int left, right;
      printf("creating point!!!\n");
      for (int i=(e[0].pSize-1); i>=0; i--) {
        if (e[0].p[i].time<(mouse.x-10)*256) {
          left=i; break;
        }
      }
      for (int i=0; i<e[0].pSize; i++) {
        if (e[0].p[i].time>(mouse.x-10)*256) {
          right=i; break;
        }
      }
      printf("left %d right %d\n",left,right);
      pResize(&e[0].p,&e[0].pSize,e[0].pSize+1);
      // special case: end of envelope
      if (left==e[0].pSize-2) {
        memset(&e[0].p[left+1],0,sizeof(envp));
        selPoint=left+1;
        selGrab=true;
      } else {
        // slide loop points
        if (e[0].susStart>=right) {
          e[0].susStart++;
        }
        if (e[0].susEnd>=right) {
          e[0].susEnd++;
        }
        if (e[0].loopStart>=right) {
          e[0].loopStart++;
        }
        if (e[0].loopEnd>=right) {
          e[0].loopEnd++;
        }
        // slide right points
        for (int i=e[0].pSize-1; i>=right; i--) {
          memcpy(&e[0].p[i],&e[0].p[i-1],sizeof(envp));
        }
        // clean up point
        memset(&e[0].p[right],0,sizeof(envp));
        selPoint=right;
        selGrab=true;
      }
      // modify point
      e[0].p[selPoint].value=fmin(1,fmax(0,(340.0f-(float)mouse.y)/300.0f));
      if (selPoint!=0) {
        if (selPoint==e[0].pSize-1) {
          e[0].p[selPoint].time=fmax(e[0].p[selPoint-1].time+1,(mouse.x-10)*256);
        } else {
          e[0].p[selPoint].time=fmax(e[0].p[selPoint-1].time+1,fmin((mouse.x-10)*256,e[0].p[selPoint+1].time-1));
        }
      }
    } else {
      // show point menu
      pMenuVis=true;
      pMenuSel=-1;
      pMenuTarget=selPoint;
      pMenuPos.x=mouse.x;
      pMenuPos.y=mouse.y;
      pMenuPos.w=108;
      pMenuPos.h=134;
    }
  }
}

void Sampler::envMouseUp(int button) {
  if (selGrab) {
    selGrab=false;
    printf("end of grab\n");
  }
}

void Sampler::drawEnvEdit() {
  tempr.x=10;  tempr1.x=0;
  tempr.y=40;  tempr1.y=0;
  tempr.w=720; tempr1.w=720;
  tempr.h=300; tempr1.h=300;
  SDL_RenderCopy(r,envbg,&tempr1,&tempr);
  tempr.x=90;  tempr1.x=0;
  tempr.y=10; tempr1.y=0;
  tempr.w=640; tempr1.w=640;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,spathlarge,&tempr1,&tempr);
  
  tempr.x=10; tempr1.x=60*sselectS;
  tempr.y=10; tempr1.y=0;
  tempr.w=70;  tempr1.w=70;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,senvsel,&tempr1,&tempr);
  f->draw(45,10,tempc,1,0,0,"Envelope");
  
  f->draw(93,10,tempc,0,0,0,"[insert name here]");
  SDL_SetRenderDrawColor(r,255,255,128,255);
  for (size_t i=0; i<e[0].pSize; i++) {
    if (i==e[0].loopStart || i==e[0].loopEnd) {
      SDL_SetRenderDrawColor(r,255,32,32,255);
      SDL_RenderDrawLine(r,10+(e[0].p[i].time/256),40,10+(e[0].p[i].time/256),340);
      SDL_SetRenderDrawColor(r,255,255,128,255);
    }
    // sustain checks //
    if (i==e[0].susStart) {
      vlineRGBA(r,3+(e[0].p[i].time/256),40,340,32,255,32,16);
      vlineRGBA(r,4+(e[0].p[i].time/256),40,340,32,255,32,32);
      vlineRGBA(r,5+(e[0].p[i].time/256),40,340,32,255,32,48);
      vlineRGBA(r,6+(e[0].p[i].time/256),40,340,32,255,32,64);
      vlineRGBA(r,7+(e[0].p[i].time/256),40,340,32,255,32,80);
      vlineRGBA(r,8+(e[0].p[i].time/256),40,340,32,255,32,96);
      vlineRGBA(r,9+(e[0].p[i].time/256),40,340,32,255,32,112);
      vlineRGBA(r,10+(e[0].p[i].time/256),40,340,32,255,32,128);
    }
    if (i==e[0].susEnd) {
      vlineRGBA(r,17+(e[0].p[i].time/256),40,340,32,255,32,16);
      vlineRGBA(r,16+(e[0].p[i].time/256),40,340,32,255,32,32);
      vlineRGBA(r,15+(e[0].p[i].time/256),40,340,32,255,32,48);
      vlineRGBA(r,14+(e[0].p[i].time/256),40,340,32,255,32,64);
      vlineRGBA(r,13+(e[0].p[i].time/256),40,340,32,255,32,80);
      vlineRGBA(r,12+(e[0].p[i].time/256),40,340,32,255,32,96);
      vlineRGBA(r,11+(e[0].p[i].time/256),40,340,32,255,32,112);
      vlineRGBA(r,10+(e[0].p[i].time/256),40,340,32,255,32,128);
    }
    // loop checks //
    if (i==e[0].loopStart) {
      vlineRGBA(r,3+(e[0].p[i].time/256),40,340,255,32,32,16);
      vlineRGBA(r,4+(e[0].p[i].time/256),40,340,255,32,32,32);
      vlineRGBA(r,5+(e[0].p[i].time/256),40,340,255,32,32,48);
      vlineRGBA(r,6+(e[0].p[i].time/256),40,340,255,32,32,64);
      vlineRGBA(r,7+(e[0].p[i].time/256),40,340,255,32,32,80);
      vlineRGBA(r,8+(e[0].p[i].time/256),40,340,255,32,32,96);
      vlineRGBA(r,9+(e[0].p[i].time/256),40,340,255,32,32,112);
      vlineRGBA(r,10+(e[0].p[i].time/256),40,340,255,32,32,128);
    }
    if (i==e[0].loopEnd) {
      vlineRGBA(r,17+(e[0].p[i].time/256),40,340,255,32,32,16);
      vlineRGBA(r,16+(e[0].p[i].time/256),40,340,255,32,32,32);
      vlineRGBA(r,15+(e[0].p[i].time/256),40,340,255,32,32,48);
      vlineRGBA(r,14+(e[0].p[i].time/256),40,340,255,32,32,64);
      vlineRGBA(r,13+(e[0].p[i].time/256),40,340,255,32,32,80);
      vlineRGBA(r,12+(e[0].p[i].time/256),40,340,255,32,32,96);
      vlineRGBA(r,11+(e[0].p[i].time/256),40,340,255,32,32,112);
      vlineRGBA(r,10+(e[0].p[i].time/256),40,340,255,32,32,128);
    }
    aacircleRGBA(r,10+(e[0].p[i].time/256),340-(e[0].p[i].value*300.0f),4,255,255,128,255);
    if (i==selPoint) {
      aacircleRGBA(r,10+(e[0].p[i].time/256),340-(e[0].p[i].value*300.0f),5,255,255,0,255);
      if (selGrab) {
        selRot=(selRot+4)%360;
        arcRGBA(r,10+(e[0].p[i].time/256),340-(e[0].p[i].value*300.0f),8,selRot,selRot+90,255,255,0,255);
        arcRGBA(r,10+(e[0].p[i].time/256),340-(e[0].p[i].value*300.0f),8,selRot+180,selRot+270,255,255,0,255);
      }
    }
    if (i<e[0].pSize-1) {
      aalineRGBA(r,10+(e[0].p[i].time/256),340-(e[0].p[i].value*300.0f),
      10+(e[0].p[i+1].time/256),340-(e[0].p[i+1].value*300.0f),255,255,128,255);
    }
  }
  
  for (size_t i=0; i<vSize; i++) {
    SDL_RenderDrawLine(r,10+(v[i].envposN/256)+(e[0].p[v[i].envpi].time/256),40,10+(v[i].envposN/256)+(e[0].p[v[i].envpi].time/256),340);
    f->drawf(10+(v[i].envposN/256)+(e[0].p[v[i].envpi].time/256),340,tempc,0,0,"%d: %d",i,v[i].envpi);
  }
  
  if (pMenuVis) {
    SDL_SetRenderDrawColor(r,0,0,0,128);
    SDL_RenderFillRect(r,&pMenuPos);
    SDL_SetRenderDrawColor(r,128,128,128,128);
    switch (pMenuSel) {
      case 0:
        tempr.x=pMenuPos.x; tempr.y=pMenuPos.y+3;
        tempr.w=106;        tempr.h=20;
        SDL_RenderFillRect(r,&tempr);
        break;
      case 1:
        tempr.x=pMenuPos.x; tempr.y=pMenuPos.y+28;
        tempr.w=106;        tempr.h=20;
        SDL_RenderFillRect(r,&tempr);
        break;
      case 2:
        tempr.x=pMenuPos.x; tempr.y=pMenuPos.y+53;
        tempr.w=106;        tempr.h=18;
        SDL_RenderFillRect(r,&tempr);
        break;
      case 3:
        tempr.x=pMenuPos.x; tempr.y=pMenuPos.y+73;
        tempr.w=106;        tempr.h=18;
        SDL_RenderFillRect(r,&tempr);
        break;
      case 4:
        tempr.x=pMenuPos.x; tempr.y=pMenuPos.y+93;
        tempr.w=106;        tempr.h=18;
        SDL_RenderFillRect(r,&tempr);
        break;
      case 5:
        tempr.x=pMenuPos.x; tempr.y=pMenuPos.y+113;
        tempr.w=106;        tempr.h=18;
        SDL_RenderFillRect(r,&tempr);
        break;
    }
    SDL_SetRenderDrawColor(r,192,192,192,255);
    SDL_RenderDrawRect(r,&pMenuPos);
    SDL_RenderDrawLine(r,pMenuPos.x+3,pMenuPos.y+25,pMenuPos.x+104,pMenuPos.y+25);
    SDL_RenderDrawLine(r,pMenuPos.x+3,pMenuPos.y+50,pMenuPos.x+104,pMenuPos.y+50);
    f->draw(pMenuPos.x+3,pMenuPos.y+3,tempc,0,0,0,"Delete");
    f->draw(pMenuPos.x+3,pMenuPos.y+28,tempc,0,0,0,"Sustain");
    f->draw(pMenuPos.x+3,pMenuPos.y+52,tempc,0,0,0,"Sustain Start");
    f->draw(pMenuPos.x+3,pMenuPos.y+72,tempc,0,0,0,"Sustain End");
    f->draw(pMenuPos.x+3,pMenuPos.y+92,tempc,0,0,0,"Loop Start");
    f->draw(pMenuPos.x+3,pMenuPos.y+112,tempc,0,0,0,"Loop End");
  }
  
  SDL_SetRenderDrawColor(r,0,0,0,255);
}
