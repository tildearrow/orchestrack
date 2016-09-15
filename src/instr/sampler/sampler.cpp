#include "sampler.h"

OTrackInsSpec* Sampler::getspec() {
  OTrackInsSpec* aspec;
  aspec=new OTrackInsSpec;
  aspec->name="Sampler";
  aspec->desc="default sampler";
  aspec->author="tildearrow";
  aspec->copyright="none";
  aspec->license="none";
  return aspec;
}

void Sampler::reset() {
  v.resize(0);
}

float* Sampler::getSample() {
  if (busy) {v.resize(0); printf("busy!\n"); return sample;}
  float element0, element1, element, definitepos;
  sample[0]=0;
  sample[1]=0;
  ev=(unsigned char*)getEvent();
  while (ev!=NULL) {
    if ((ev[0]>>4)==8) {
      // find voice with properties, then destroy it
      for (int i=0; i<v.size(); i++) {
        if (v[i].chan==(ev[0]&15)) {
          if (v[i].note==ev[1]) {
            printf("erased. %d %.2x %.2x\n",i,v[i].chan,v[i].note);
            v.erase(v.begin()+i); i--;
          }
        }
      }
      /*
      v[ev[0]&15].period=0;
      v[ev[0]&15].f=0;
      v[ev[0]&15].vol=(float)ev[2]/128;*/
    }
    if ((ev[0]>>4)==9) {
      // allocate a voice
      v.resize(v.size()+1);
      int thisv;
      thisv=v.size()-1;
      v[thisv].chan=ev[0]&15;
      v[thisv].note=ev[1];
      printf("allocated. %d %.2x %.2x\n",thisv,v[thisv].chan,v[thisv].note);
      v[thisv].period=0;
      v[thisv].f=pow(2,((float)v[thisv].note-60)/12)*s[0].rate/44100;
      v[thisv].vol=(float)ev[2]/128;
    }
    if ((ev[0]>>4)==0xe) {
      // pitch bend.
      c[ev[0]&15].pitch=(ev[1]+(ev[2]<<7))-0x2000;
      printf("pitch %d\n",c[ev[0]&15].pitch);
      for (int i=0; i<v.size(); i++) {
        if (v[i].chan==(ev[0]&15)) {
          v[i].f=pow(2,((float)v[i].note-60+((float)c[ev[0]&15].pitch/4096.0))/12)*s[0].rate/44100;
        }
      }
    }
    if ((ev[0]>>3)==0x1e) {
      // SC messages
      switch (ev[0]&15) {
        case 2: tick=ev[1]+(ev[1]<<7); printf("pos\n"); break;
      }
    }
    if ((ev[0]>>3)==0x1f) {
      // real-time messages.
      switch (ev[0]&15) {
        case 8: tick++; break;
        case 0xa: tick=0; break;
        //case 0xc: tick=0; break;
        case 0xf: reset(); break;
      }
    }
    ev=(unsigned char*)getEvent();
  }
  for (int i=0; i<v.size(); i++) {
    definitepos=fmin(v[i].period,s[0].len);
    element0=s[0].data[(int)definitepos];
    element1=s[0].data[(int)definitepos+1];
    element=element0+((element1-element0)*fmod(definitepos,1));
    
    sample[0]+=element*v[i].vol;
    sample[1]+=element*v[i].vol;
    v[i].period+=v[i].f;
  }
  
  for (int i=0; i<v.size(); i++) {
    if (v[i].period>s[0].len) {
      printf("sample finished. %d\n",i);
      v.erase(v.begin()+i); i--;
    }
  }
  sample[0]=sample[0]/4;
  sample[1]=sample[1]/4;
  return sample;
}

string Sampler::topLevel(string path) {
  string res;
  res=path.erase(path.find_last_of(DIR_SEP),path.size()-path.find_last_of(DIR_SEP));
#ifdef _WIN32
  if (res.size()==2 && res.at(0)>='A' && res.at(0)<='Z' && res.at(1)==':') {
    res+="\\";
  }
#else
  if (strcmp(res.c_str(),"")==0) {
    res="/";
  }
#endif
  return res;
}

#ifdef _WIN32
int Sampler::readDir(const char* path) {
  HANDLE od;
  WIN32_FIND_DATA* de;
  dentry dede;
  de=new WIN32_FIND_DATA;
  string apath;
  apath=path;
  apath+="\\*";
  od=FindFirstFile(apath.c_str(),de);
  dentry tempelem;
  if (od!=NULL) {
    listings.resize(0);
    while (1) {
      if (FindNextFile(od,de)==0) {
        break;
      } else {
        if (strcmp(".",de->cFileName)!=0 && strcmp("..",de->cFileName)!=0) {
          if (showHidden || (!(de->dwFileAttributes&FILE_ATTRIBUTE_HIDDEN))) {
            dede.name=de->cFileName;
            dede.type=(de->dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)?(4):(8);
            listings.push_back(dede);
          }
        }
      }
    }
    FindClose(od);
    delete de;
    return 1;
  }
  return 0;
}
#else
int Sampler::readDir(const char* path) {
  DIR* od;
  ////// king
  dirent* de;
  dentry dede;
  od=opendir(path);
  dentry tempelem;
  if (od!=NULL) {
    listings.clear();
    while (1) {
      de=readdir(od);
      if (!de) {
        break;
      } else {
        if (strcmp(".",de->d_name)!=0 && strcmp("..",de->d_name)!=0) {
          if (showHidden || (de->d_name[0]!='.')) {
            dede.name=de->d_name;
            dede.type=de->d_type;
            listings.push_back(dede);
          }
        }
      }
    }
    closedir(od);
    for (int i=0; i<listings.size(); i++) {
      for (int j=0; j<listings.size()-1; j++) {
        if (strcmp(listings[j].name.c_str(),listings[j+1].name.c_str())>0) {
          tempelem=listings[j];
          listings[j]=listings[j+1];
          listings[j+1]=tempelem;
        }
      }
    }
    return 1;
  } else {
    return 0;
  }
}
#endif

void Sampler::mouseEvent(int type, int button, int x, int y, int finger) {
  switch (type) {
    case 0:
      mouse.x=x; mouse.y=y;
      if (curView==2) {
        if (PointInRect(mouse.x,mouse.y,690,10,690+40,10+20)) {
          if (sloadS!=2) {
            sloadS=1;
          }
        } else {
          if (sloadS!=2) {
            sloadS=0;
          }
        }
        if (PointInRect(mouse.x,mouse.y,630,40,630+40,40+20)) {
          if (seupS!=2) {
            seupS=1;
          }
        } else {
          if (seupS!=2) {
            seupS=0;
          }
        }
        if (PointInRect(mouse.x,mouse.y,580,40,580+40,40+20)) {
          if (sedownS!=2) {
            sedownS=1;
          }
        } else {
          if (sedownS!=2) {
            sedownS=0;
          }
        }
      }
      if (showLoad) {
        if (PointInRect(mouse.x,mouse.y,30,30,30+40,30+20)) {
          if (supS!=2) {
            supS=1;
          }
        } else {
          if (supS!=2) {
            supS=0;
          }
        }
      }
      if (touching) {
        listPos=-mouse.y+touchSPos;
        scrolling=true;
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
      if (curView==2) {
        if (PointInRect(mouse.x,mouse.y,690,10,690+40,10+20)) {
          sloadS=2;
        }
        if (PointInRect(mouse.x,mouse.y,630,40,630+40,40+20)) {
          seupS=2;
          if (button!=1) {
            doUp=true;
          } else {
            s[0].rate*=2;
          }
        }
        if (PointInRect(mouse.x,mouse.y,580,40,580+40,40+20)) {
          sedownS=2;
          if (button!=1) {
            doDown=true;
          } else {
            s[0].rate/=2;
          }
        }
      }
      if (showLoad) {
        if (PointInRect(mouse.x,mouse.y,30,30,30+40,30+20)) {
          supS=2;
        }
        if (PointInRect(mouse.x,mouse.y,33,63,33+674,63+392/*listings.size()*20*/)) {
          // swipe code
          touching=true;
          scrolling=false;
          touchSPos=listPos+mouse.y;
          listSpeed=0;
        }
      }
      break;
    case 1:
      mouse.b[button]=0;
      if (showLoad) {
        if (touching) {
          touching=false;
          listSpeed=fabs(polledMY-oldPolledMY);
          listDir=(polledMY-oldPolledMY)>0;
          if (listPos<0 || (listPos+392)>20*(listings.size())) {
            listSpeed=0;
          }
        }
        if (PointInRect(mouse.x,mouse.y,33,63,33+674,63+392)) {
          if (!scrolling) {
            if (loadHIndex==(int)((mouse.y-63+listPos)/20) && loadHIndex<listings.size()) {
              if (listings[loadHIndex].type==4) {
                if (wd.at(wd.size()-1)!=DIR_SEP) {
                  wd+=DIR_SEP;
                }
                wd+=listings[loadHIndex].name;
                readDir(wd.c_str());
                listPos=0;
              } else if (listings[loadHIndex].type==8) {
                // try to load sample
                string path;
                path=wd;
                path+=DIR_SEP;
                path+=listings[loadHIndex].name;
                printf("opening %s\n",path.c_str());
                sndf=sf_open(path.c_str(),SFM_READ,&si);
                if (sf_error(sndf)==SF_ERR_NO_ERROR) {
                  printf("loading sample...\n");
                  busy=true;
                  v.resize(0);
                  s[0].len=si.frames;
                  s[0].chan=si.channels;
                  s[0].rate=si.samplerate;
                  delete[] s[0].data;
                  s[0].data=new float[si.frames*si.channels];
                  sf_read_float(sndf,s[0].data,si.frames);
                  s[0].path=listings[loadHIndex].name;
                  printf("finished.\n");
                  sf_close(sndf);
                  showLoad=false;
                  busy=false;
                } else {
                  sf_perror(sndf);
                }
              }
            } else {
              loadHIndex=(!PointInRect(mouse.x,mouse.y,30,60,30+680,60+392))?(-1):((mouse.y-63+listPos)/20);
            }
          } else {
            scrolling=false;
          }
        }
      }
      if (curView==2) {
        if (sloadS!=1) {
          sloadS=PointInRect(mouse.x,mouse.y,690,10,690+40,10+20);
          if (sloadS) {
            printf("load?\n");
            readDir(wd.c_str());
            showLoad=true;
          }
        }
        if (seupS!=1) {
          seupS=PointInRect(mouse.x,mouse.y,630,40,630+40,40+20);
          doUp=false;
        }
        if (sedownS!=1) {
          sedownS=PointInRect(mouse.x,mouse.y,580,40,580+40,40+20);
          doDown=false;
        }
      }
      if (supS!=1) {
        supS=PointInRect(mouse.x,mouse.y,30,30,30+40,30+20);
        if (supS && showLoad) {
          printf("goes up\n");
          wd=topLevel(wd);
          readDir(wd.c_str());
          listPos=0;
        }
      }
      break;
    case 3:
      if (showLoad) {
        if (y==0) {
          break;
        }
#ifdef __APPLE__
        listSpeed+=fabs((float)y)*2;
#else
        listSpeed+=fabs((float)y)*8;
#endif
        listDir=(y>0)?(1):(0);
      }
      break;
  }
  
}

void Sampler::setRenderer(SDL_Renderer* renderer) {
  r=renderer;
  f=new font;
  f->setrenderer(r);
  #ifdef __linux__
  f->load("/usr/share/fonts/TTF/Ubuntu-R.ttf",16);
  #elif __APPLE__
  f->load("/System/Library/Fonts/Helvetica.dfont",16);
  #elif _WIN32
  f->load("C:\\Windows\\Fonts\\segoeui.ttf",16);
  #endif
  // init UI
  tempc.r=0;
  tempc.g=128;
  tempc.b=255;
  tempc.a=255;
  but=drawButton(r,0,0,128,64,tempc,16);
  tempc.r=34;
  tempc.g=68;
  tempc.b=98;
  tempc.a=255;
  grid=drawButton(r,0,0,720,340,tempc,4);
  tempc.r=16;
  tempc.g=16;
  tempc.b=16;
  tempc.a=255;
  spath=drawButton(r,0,0,600,20,tempc,4);
  slfdir=drawButton(r,0,0,580,20,tempc,4);
  slfpath=drawButton(r,0,0,570,20,tempc,4);
  tempc.r=64;
  tempc.g=64;
  tempc.b=64;
  tempc.a=255;
  sload=drawButton(r,0,0,40,20,tempc,4);
  scancel=drawButton(r,0,0,50,20,tempc,4);
  tempc.r=32;
  tempc.g=32;
  tempc.b=32;
  tempc.a=255;
  sloadform=drawButton(r,0,0,700,472,tempc,10);
  tempc.r=48;
  tempc.g=48;
  tempc.b=48;
  tempc.a=255;
  slflist=drawButton(r,0,0,680,392,tempc,2);
  tempc.r=48;
  tempc.g=48;
  tempc.b=48;
  tempc.a=255;
  smode=drawButton(r,0,0,175,20,tempc,2);
  tempc.r=128;
  tempc.g=128;
  tempc.b=128;
  tempc.a=255;
  smodeactive=drawButton(r,0,0,175,20,tempc,4);
  showLoad=false;
}

void Sampler::drawLoadUI() {
  tempr.x=0;
  tempr.y=0;
  tempr.w=740;
  tempr.h=512;
  SDL_SetRenderDrawColor(r,0,0,0,192);
  SDL_RenderFillRect(r,&tempr);
  
  tempr.x=20; tempr1.x=0;
  tempr.y=20; tempr1.y=0;
  tempr.w=700;  tempr1.w=700;
  tempr.h=472;  tempr1.h=472;
  SDL_RenderCopy(r,sloadform,&tempr1,&tempr);
  
  tempr.x=30; tempr1.x=0;
  tempr.y=60; tempr1.y=0;
  tempr.w=680;  tempr1.w=680;
  tempr.h=392;  tempr1.h=392;
  SDL_RenderCopy(r,slflist,&tempr1,&tempr);
  
  tempr.x=80; tempr1.x=0;
  tempr.y=30; tempr1.y=0;
  tempr.w=580;  tempr1.w=580;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,slfdir,&tempr1,&tempr);
  
  tempr.x=670; tempr1.x=0;
  tempr.y=30; tempr1.y=0;
  tempr.w=40;  tempr1.w=40;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,sload,&tempr1,&tempr);
  
  tempr.x=30; tempr1.x=40*supS;
  SDL_RenderCopy(r,sload,&tempr1,&tempr);
  
  tempr.y=462; tempr1.y=0;
  
  tempr.x=610; tempr1.x=0;
  SDL_RenderCopy(r,sload,&tempr1,&tempr);
  
  tempr.x=660; tempr1.x=0;
  tempr.w=50;  tempr1.w=50;
  SDL_RenderCopy(r,scancel,&tempr1,&tempr);
  
  tempr.x=30; tempr1.x=0;
  tempr.y=462; tempr1.y=0;
  tempr.w=570;  tempr1.w=570;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,slfpath,&tempr1,&tempr);
  
  f->draw(50,30,tempc,1,0,0,"Up");
  f->draw(690,30,tempc,1,0,0,"MD");
  
  f->draw(630,462,tempc,1,0,0,"Load");
  f->draw(685,462,tempc,1,0,0,"Cancel");
  
  f->draw(83,30,tempc,0,0,0,wd);
  //f->draw(33,462,tempc,0,0,0,"filename.wav");
  
  SDL_Rect clipr;
  clipr.x=32; clipr.y=62; clipr.w=676; clipr.h=388;
  SDL_RenderSetClipRect(r,&clipr);
  if (loadHIndex!=-1 && loadHIndex<listings.size()) {
    SDL_SetRenderDrawColor(r,255,255,255,(!scrolling && touching && loadHIndex==(int)((mouse.y-63+listPos)/20))?(128):(64));
    tempr.x=33;
    tempr.y=66+20*loadHIndex-listPos;
    tempr.w=674;
    tempr.h=20;
    SDL_RenderFillRect(r,&tempr);
  }

  for (int i=fmax(0,listPos/20); i<fmin(listings.size(),20+listPos/20); i++) {
    switch (listings[i].type) {
      case 1: tempc.r=255; tempc.g=192; tempc.b=160; break; // fifo
      case 2: tempc.r=255; tempc.g=255; tempc.b=160; break; // character
      case 4: tempc.r=160; tempc.g=192; tempc.b=255; break; // directory
      case 6: tempc.r=255; tempc.g=220; tempc.b=160; break; // block
      case 8: tempc.r=255; tempc.g=255; tempc.b=255; break; // file
      case 10: tempc.r=160; tempc.g=220; tempc.b=255; break; // link
      case 12: tempc.r=255; tempc.g=128; tempc.b=255; break; // socket
      default: tempc.r=64; tempc.g=64; tempc.b=64; break; // unknown
    }
    f->draw(33,66+(20*i)-listPos,tempc,0,0,0,listings[i].name);
  }
  
  SDL_RenderSetClipRect(r,NULL);
  
  oldPolledMY=polledMY;
  polledMY=mouse.y;
  
  if (touching) {
    //printf("fSpeed: %f\n",polledMY-oldPolledMY);
  } else {
    if (listDir) {
      listPos-=listSpeed;
    } else {
      listPos+=listSpeed;
    }
    if ((listPos+392)>20*(listings.size()) || listPos<0) {
      listSpeed*=0.6;
    } else {
      listSpeed*=0.9;
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
    //printf("listPos: %f\n",listPos);
  }
  
  if (listPos>0 && (listPos+392)>20*(listings.size()) && !touching) {
    if (392>20*listings.size()) {
      listPos+=-listPos/8;
      if (listPos<0.5) {
        listPos=0;
      }
    } else {
      listPos-=((listPos+392)-20*listings.size())/8;
      if (listPos<((20*(float)listings.size())+0.5-392)) {
        listPos=(20*(float)listings.size()-392);
      }
    }
    //printf("listPos: %f. target %f\n",listPos,((20*(float)listings.size()-392)+0.5));
  }

}

void Sampler::drawSummary() {
  
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
  //SDL_RenderCopy(r,but,NULL,&tempr);
  // draw currently playing notes
  SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_ADD);
  SDL_SetRenderDrawColor(r,255,255,255,32);
  for (int i=0; i<v.size(); i++) {
    tempr.x=v[i].note*4;
    tempr.y=10;
    tempr.w=4;
    tempr.h=340;
    SDL_RenderFillRect(r,&tempr);
  }
  SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
  f->draw(370,16,tempc,1,0,0,"Note");
  f->draw(0,200,tempc,1,1,0,"NoteVol");
  f->drawf(83,400,tempc,0,0,"%d %d %d %d",tick/96,(tick/24)%4,(tick/6)%4,tick%6);
}

void Sampler::drawSampleEdit() {
  f->draw(10,10,tempc,0,0,0,"Sample");
  f->draw(10,40,tempc,0,0,0,"Rate");
  tempr.x=80;  tempr1.x=0;
  tempr.y=10; tempr1.y=0;
  tempr.w=600; tempr1.w=600;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,spath,&tempr1,&tempr);
  tempr.x=690; tempr1.x=40*sloadS;
  tempr.y=10; tempr1.y=0;
  tempr.w=40;  tempr1.w=40;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,sload,&tempr1,&tempr);
  tempr.x=630; tempr1.x=40*seupS;
  tempr.y=40; tempr1.y=0;
  SDL_RenderCopy(r,sload,&tempr1,&tempr);
  tempr.x=580; tempr1.x=40*sedownS;
  SDL_RenderCopy(r,sload,&tempr1,&tempr);
  tempr.x=680; tempr1.x=50;
  tempr.y=40; tempr1.y=0;
  tempr.w=50;  tempr1.w=50;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,scancel,&tempr1,&tempr);
  f->draw(83,10,tempc,0,0,0,s[0].path);
  f->drawf(83,40,tempc,0,0,"%f",s[0].rate);
  f->draw(710,10,tempc,1,0,0,"Load");
  f->draw(705,40,tempc,1,0,0,"Keypad");
  f->draw(650,40,tempc,1,0,0,"Up");
  f->draw(600,40,tempc,1,0,0,"Down");
  if (doUp) {
    if (timeOnButton%(int)fmax(64-timeOnButton,1)==0) {
      s[0].rate+=(int)fmax(1,pow(10,(float)timeOnButton/128)/10);
    }
    timeOnButton++;
  }
  if (doDown) {
    if (timeOnButton%(int)fmax(64-timeOnButton,1)==0) {
      s[0].rate-=(int)fmax(1,pow(10,(float)timeOnButton/128)/10);
    }
    timeOnButton++;
  }
  if (!(doUp || doDown)) {
    timeOnButton=0;
  }
}

void Sampler::drawUI() {
  tempc.r=255;
  tempc.g=255;
  tempc.b=255;
  tempc.a=255;
  
  SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
  
  tempr.x=8;  tempr1.x=0;
  tempr.y=482; tempr1.y=0;
  tempr.w=175; tempr1.w=175;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,(curView==0)?(smodeactive):(smode),&tempr1,&tempr);
  
  tempr.x=191;  tempr1.x=0;
  SDL_RenderCopy(r,(curView==1)?(smodeactive):(smode),&tempr1,&tempr);
  
  tempr.x=374;  tempr1.x=0;
  SDL_RenderCopy(r,(curView==2)?(smodeactive):(smode),&tempr1,&tempr);
  
  tempr.x=557;  tempr1.x=0;
  SDL_RenderCopy(r,(curView==3)?(smodeactive):(smode),&tempr1,&tempr);
  
  f->draw(95,482,tempc,1,0,0,"Summary");
  f->draw(278,482,tempc,1,0,0,"Grid");
  f->draw(462,482,tempc,1,0,0,"Editor");
  f->draw(644,482,tempc,1,0,0,"Envelope");
  
  switch (curView) {
    case 0: drawSummary(); break;
    case 1: drawGrid(); break;
    case 2: drawSampleEdit(); break;
  }
  
  if (showLoad) {
    drawLoadUI();
  }
  
  SDL_SetRenderDrawColor(r,(mouse.b[0])?(0):(255),(mouse.b[1])?(0):(255),(mouse.b[2])?(0):(255),255);
  SDL_RenderDrawLine(r,mouse.x,mouse.y,0,0);
  SDL_SetRenderDrawColor(r,0,0,0,255);
}

bool Sampler::init(int inChannels, int outChannels) {
  if (inChannels==0) {
    sample=new float[outChannels];
    outCh=outChannels;
    /*v.resize(16);
    for (int i=0; i<v.size(); i++) {
      v[i].period=0;
    }*/
    char* twd;
#ifndef _WIN32
    twd=getcwd(NULL,PATH_MAX);
#else
    twd=_getcwd(NULL,MAX_PATH);
#endif
    wd=twd;
    delete[] twd;
    s.resize(1);
    s[0].path="../share/orchestrack/testsmp.wav";
    sndf=sf_open(s[0].path.c_str(),SFM_READ,&si);
    s[0].len=si.frames;
    s[0].chan=si.channels;
    s[0].rate=si.samplerate;
    s[0].data=new float[si.frames*si.channels];
    sf_read_float(sndf,s[0].data,si.frames);
    sf_close(sndf);
    showHidden=false;
    busy=false;
    return true;
  } else {
    return false;
  }
}
