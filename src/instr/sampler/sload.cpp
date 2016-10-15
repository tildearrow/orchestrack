#include "sampler.h"

void Sampler::initSample(int which) {
  char* sl;
  printf("which: %d\n",which);
  s[which].path=new string;
  s[which].path[0]="Sample";
  sl=new char[21];
  sprintf(sl,"%d",which+1);
  s[which].path[0]+=sl;
  delete[] sl;
  s[which].len=128;
  s[which].chan=1;
  s[which].rate=33452;
  s[which].noteMin=0;
  s[which].noteMax=127;
  s[which].velMin=0;
  s[which].velMax=127;
  s[which].data=new float*[1];
  s[which].loopStart=0;
  s[which].loopEnd=127;
  s[which].loopType=1;
  s[which].data[0]=new float[128+16];
  for (int i=0; i<128+16; i++) {
    s[which].data[0][i]=0;
  }
  for (int i=8; i<128+8; i++) {
    s[which].data[0][i]=((float)i/256)-1;
  }
  s[which].volAmt=1;
  s[which].volCap=0;
}

void Sampler::loadSample() {
  if (listings[loadHIndex].type==4) {
    string temp;
    temp=wd;
    if (temp.at(temp.size()-1)!=DIR_SEP) {
      temp+=DIR_SEP;
    }
    temp+=listings[loadHIndex].name;
    if (readDir(temp.c_str())) {
      int sx;
      TTF_SizeUTF8(f->f,wd.c_str(),&sx,NULL);
      wd=temp;
      listPos=0;
      loadHIndex=-1;
      sfname="";
      if (wdoff>sx-573 && wddir==1) {
        wddir=0;
      }
      wdoff=(int)fmax(0,fmin(sx-573,wdoff));
    } else {
      perror("can't read dir");
      erra=64;
      errl[0]=friendlyErr0(errno);
      errl[1]=friendlyErr1(errno);
      errl[2]=friendlyErr2(errno);
    }
  } else if (listings[loadHIndex].type==8) {
    // try to load sample
    string path;
    path=wd;
    path+=DIR_SEP;
    path+=listings[loadHIndex].name;
    printf("opening %s\n",path.c_str());
    sndf=sf_open(path.c_str(),SFM_READ,&si);
    int sfe;
    sfe=sf_error(sndf);
    if (sfe==SF_ERR_NO_ERROR) {
      printf("loading sample...\n");
      busy=true;
      vResize(0);
      s[curSample].len=(int)si.frames;
      for (int i=0; i<s[curSample].chan; i++) {
        delete[] s[curSample].data[i];
      }
      delete[] s[curSample].data;
      s[curSample].chan=si.channels;
      s[curSample].rate=(float)si.samplerate;
      s[curSample].data=new float*[si.channels];
      tbuf=new float[si.channels];
      for (int i=0; i<si.channels; i++) {
        s[curSample].data[i]=new float[(size_t)si.frames+16];
        for (int j=0; j<si.frames+16; j++) {
          s[curSample].data[i][j]=0;
        }
      }
      for (int i=8; i<si.frames+8; i++) {
        sf_readf_float(sndf,tbuf,1);
        for (int j=0; j<si.channels; j++) {
          s[curSample].data[j][i]=tbuf[j];
        }
      }
      sf_close(sndf);
      delete[] tbuf;
      FILE* fo;
      fo=fopen(path.c_str(),"rb");
      lf=readIFF(fo);
      //printf("iff: %d\n",lf);
      fclose(fo);
      if (lf!=NULL) {
        if (lf->isRIFF) {
          lwf=readWAVE(lf);
          printf("%ld\n",lwf->smpl.loops);
          if ((lwf->smpl.loops)>0) {
            s[curSample].loopType=1;
            s[curSample].loopStart=lwf->smpl.l[0].start;
            s[curSample].loopEnd=lwf->smpl.l[0].end;
          } else {
            s[curSample].loopType=0;
            s[curSample].loopStart=0;
            s[curSample].loopEnd=0;
          }
          freeWAVE(lwf);
        } else {
          int candidate;
          laf=readAIFF(lf);
          if (laf->inst.sloop.mode>0) {
            candidate=0;
            s[curSample].loopType=1;
            for (size_t i=0; i<laf->m.size(); i++) {
              if (laf->m[i].id==laf->inst.sloop.start) {
                candidate=i; break;
              }
            }
            s[curSample].loopStart=laf->m[candidate].pos;
            for (size_t i=0; i<laf->m.size(); i++) {
              if (laf->m[i].id==laf->inst.sloop.end) {
                candidate=i; break;
              }
            }
            s[curSample].loopEnd=laf->m[candidate].pos;
          } else {
            s[curSample].loopType=0;
            s[curSample].loopStart=0;
            s[curSample].loopEnd=0;
          }
          freeAIFF(laf);
        }
        freeIFF(lf);
      } else {
        s[curSample].loopType=0;
        s[curSample].loopStart=0;
        s[curSample].loopEnd=0;
      }
      
      s[curSample].path[0]=listings[loadHIndex].name.erase(listings[loadHIndex].name.find_last_of('.'),listings[loadHIndex].name.size()-listings[loadHIndex].name.find_last_of('.'));
      showLoad=false;
      busy=false;
    } else {
      int ee;
      switch (sfe) {
        case SF_ERR_NO_ERROR: ee=0; break;
        case SF_ERR_UNRECOGNISED_FORMAT: case 21: ee=256; break;
        case SF_ERR_SYSTEM: ee=errno; break;
        case SF_ERR_MALFORMED_FILE: ee=257; break;
        case SF_ERR_UNSUPPORTED_ENCODING: ee=258; break;
        default: ee=0; fprintf(stderr,"huh?\n"); break;
      }
      erra=64;
      errl[0]=friendlyErr0(ee);
      errl[1]=friendlyErr1(ee);
      errl[2]=friendlyErr2(ee);
    }
  }
}

void Sampler::loadMouseMove(int button) {
  hover(30,30,30+40,30+20,&supS);
  hover(660,462,660+50,462+20,&scancelS);
  listMouseMove(button);
}

void Sampler::loadMouseDown(int button) {
  if (PointInRect(mouse.x,mouse.y,30,30,30+40,30+20)) {
    supS=2;
  }
  if (PointInRect(mouse.x,mouse.y,660,462,660+50,462+20)) {
    scancelS=2;
  }
  listMouseDown(button);
}

void Sampler::loadMouseUp(int button) {
  listMouseUp(button);
}

void Sampler::drawLoadUI() {
  int sx, sy;
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
  
  tempr.x=660; tempr1.x=50*scancelS;
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
  
  TTF_SizeUTF8(f->f,wd.c_str(),&sx,&sy);
  SDL_Rect clipr;
  clipr.x=83; clipr.y=30; clipr.w=573; clipr.h=20;
  SDL_RenderSetClipRect(r,&clipr);
  f->draw(83-(int)fmax(0,fmin(sx-573,wdoff)),30,tempc,0,0,0,wd);
  SDL_RenderSetClipRect(r,NULL);
  f->draw(33,462,tempc,0,0,0,sfname);
  drawList();
  
  tempc.r=255; tempc.g=255; tempc.b=255; tempc.a=erra*2;
  
  SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
  
  if (erra>0) {
    f->draw(370,236,tempc,1,1,0,errl[0]);
    f->draw(370,256,tempc,1,1,0,errl[1]);
    f->draw(370,276,tempc,1,1,0,errl[2]);
    erra--;
    if (erra<0) {
      erra=0;
    }
  }
  
  if (sx>564) {
    if (wddir) {
      wdoff--;
      if (wdoff<-50) {
        wddir=0;
      }
    } else {
      wdoff++;
      if (wdoff>sx-523) {
        wddir=1;
      }
    }
  } else {
    wddir=0; wdoff=0;
  }
}
