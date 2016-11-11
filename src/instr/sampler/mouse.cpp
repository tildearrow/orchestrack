#include "sampler.h"

void Sampler::mouseEvent(int type, int button, int x, int y, int finger) {
  switch (type) {
    case 0:
      mouse.x=x; mouse.y=y;
      if (curView==0) {
        sumMouseMove(button);
      }
      if (curView==1) {
        grMouseMove(button);
      }
      if (curView==2) {
        seMouseMove(button);
      }
      if (curView==3) {
        envMouseMove(button);
      }
      if (showLoad || showSampleSel || showEnvSel) {
        loadMouseMove(button);
      }
      break;
    case 2: // down
      mouse.x=x; mouse.y=y;
      mouse.b[button]=1;
      if (PointInRect(mouse.x,mouse.y,8,482,8+175,482+20)) {
        curView=0;
      }
      if (PointInRect(mouse.x,mouse.y,191,482,191+175,482+20)) {
        curView=1;
      }
      if (PointInRect(mouse.x,mouse.y,374,482,374+175,482+20)) {
        curView=2;
      }
      if (PointInRect(mouse.x,mouse.y,557,482,557+175,482+20)) {
        curView=3;
      }
      if (curView==0) {
        sumMouseDown(button);
      }
      if (curView==1) {
        grMouseDown(button);
      }
      if (curView==2) {
        seMouseDown(button);
      }
      if (curView==3) {
        envMouseDown(button);
      }
      if (showLoad || showSampleSel || showEnvSel) {
        loadMouseDown(button);
      }
      break;
    case 1: // up
      mouse.b[button]=0;
      if (showLoad || showSampleSel || showEnvSel) {
        loadMouseUp(button);
        if (supS!=1) {
          supS=PointInRect(mouse.x,mouse.y,30,30,30+40,30+20);
          if (supS && (showLoad || showSampleSel || showEnvSel)) {
            if (showSampleSel) {
              sResize(sSize+1);
              int ssize1=sSize-1;
              initSample(ssize1);
              prepareSampleSel();
            } else if (showEnvSel) {
              eResize(eSize+1);
              int esize1=eSize-1;
              initEnv(esize1);
              prepareEnvSel(false);
            } else {
              printf("goes up\n");
              wd=topLevel(wd);
              readDir(wd.c_str());
              listPos=0;
            }
          }
          if (scancelS!=1) {
            scancelS=PointInRect(mouse.x,mouse.y,660,462,660+50,462+20);
            if (scancelS) {
              printf("cancel\n");
              showLoad=false;
              showSampleSel=false;
              showEnvSel=false;
            }
          }
          if (slloadS!=1) {
            slloadS=PointInRect(mouse.x,mouse.y,610,462,610+40,462+20);
            if (slloadS) {
              printf("load.\n");
              loadAction();
            }
          }
        }
        break;
      }
      if (curView==0) {
        sumMouseUp(button);
      }
      if (curView==1) {
        grMouseUp(button);
      }
      if (curView==2) {
        seMouseUp(button);
      }
      if (curView==3) {
        envMouseUp(button);
      }
      break;
    case 3:
      if (showLoad || showSampleSel || showEnvSel) {
        listMouseWheel(x,y);
      }
      break;
  }
}

void Sampler::hover(int x, int y, int x2, int y2, int* result) {
  if (PointInRect(mouse.x,mouse.y,x,y,x2,y2)) {
    if ((*result)!=2) {
      (*result)=1;
    }
  } else {
    if ((*result)!=2) {
      (*result)=0;
    }
  }
}

void Sampler::upDown() {
  if (doUp) {
    if (timeOnButton%(int)fmax(64-timeOnButton,1)==0) {
      switch (doXTarget) {
        case 0:
          s[curSample].rate+=(int)fmax(1,pow(10,(float)timeOnButton/128)/10);
          break;
        case 1:
          s[curSample].noteMin=(char)fmin(s[curSample].noteMin+1,s[curSample].noteMax);
          break;
        case 2:
          s[curSample].noteMax=(char)fmin(s[curSample].noteMax+1,127);
          break;
        case 3:
          s[curSample].velMin=(char)fmin(s[curSample].velMin+1,s[curSample].velMax);
          break;
        case 4:
          s[curSample].velMax=(char)fmin(s[curSample].velMax+1,127);
          break;
      }
    }
    timeOnButton++;
  }
  if (doDown) {
    if (timeOnButton%(int)fmax(64-timeOnButton,1)==0) {
      switch (doXTarget) {
        case 0:
          s[curSample].rate-=(int)fmax(1,pow(10,(float)timeOnButton/128)/10);
          break;
        case 1:
          s[curSample].noteMin=(char)fmax(s[curSample].noteMin-1,0);
          break;
        case 2:
          s[curSample].noteMax=(char)fmax(s[curSample].noteMax-1,s[curSample].noteMin);
          break;
        case 3:
          s[curSample].velMin=(char)fmax(s[curSample].velMin-1,0);
          break;
        case 4:
          s[curSample].velMax=(char)fmax(s[curSample].velMax-1,s[curSample].velMin);
          break;
      }
    }
    s[curSample].rate=fmax(0.0f,s[curSample].rate);
    timeOnButton++;
  }
  if (!(doUp || doDown)) {
    timeOnButton=0;
  }
}
