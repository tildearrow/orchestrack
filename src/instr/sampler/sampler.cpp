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
  if (busy) {v.resize(0); return sample;}
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
      v[thisv].sample=0;
      for (int i=0; i<s.size(); i++) {
        if (s[i].noteMin<=v[thisv].note && s[i].noteMax>=v[thisv].note &&
            s[i].velMin<=ev[2] && s[i].velMax>=ev[2]) {
          v[thisv].sample=i; break;
        }
      }
      v[thisv].f=pow(2,((float)v[thisv].note-60)/12)*s[v[thisv].sample].rate/44100;
      v[thisv].vol=(float)ev[2]/128;
    }
    if ((ev[0]>>4)==0xe) {
      // pitch bend.
      c[ev[0]&15].pitch=(ev[1]+(ev[2]<<7))-0x2000;
      printf("pitch %d\n",c[ev[0]&15].pitch);
      for (int i=0; i<v.size(); i++) {
        if (v[i].chan==(ev[0]&15)) {
          v[i].f=pow(2,((float)v[i].note-60+((float)c[ev[0]&15].pitch/4096.0))/12)*s[v[i].sample].rate/44100;
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
    if (s[v[i].sample].chan==1) {
      definitepos=fmin(v[i].period,s[v[i].sample].len);
      element0=s[v[i].sample].data[0][(int)definitepos];
      element1=s[v[i].sample].data[0][(int)definitepos+1];
      element=element0+((element1-element0)*fmod(definitepos,1));
      
      sample[0]+=element*v[i].vol;
      sample[1]+=element*v[i].vol;
    } else for (int j=0; j<s[v[i].sample].chan; j++) {
      definitepos=fmin(v[i].period,s[v[i].sample].len);
      element0=s[v[i].sample].data[j][(int)definitepos];
      element1=s[v[i].sample].data[j][(int)definitepos+1];
      element=element0+((element1-element0)*fmod(definitepos,1));
      
      sample[j]+=element*v[i].vol;
    }
    v[i].period+=v[i].f;
  }
  
  for (int i=0; i<v.size(); i++) {
    if (v[i].period>s[v[i].sample].len) {
      printf("sample finished. %d\n",i);
      v.erase(v.begin()+i); i--;
    }
  }
  sample[0]=sample[0]/4;
  sample[1]=sample[1]/4;
  return sample;
}

void Sampler::prepareSampleSel() {
  clearList();
  for (int i=0; i<s.size(); i++) {
    feedList(s[i].path,255,255,255,255);
  }
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
    clearList();
    for (int i=0; i<listings.size(); i++) {
      switch (listings[i].type) {
        case 1: feedList(listings[i].name,255,192,160,255); break; // fifo
        case 2: feedList(listings[i].name,255,255,160,255); break; // character
        case 4: feedList(listings[i].name,160,192,255,255); break; // directory
        case 6: feedList(listings[i].name,255,220,160,255); break; // block
        case 8: feedList(listings[i].name,255,255,255,255); break; // file
        case 10: feedList(listings[i].name,160,220,255,255); break; // link
        case 12: feedList(listings[i].name,255,128,255,255); break; // socket
        default: feedList(listings[i].name,128,128,128,255); break; // unknown
      }
    }
    return 1;
  } else {
    return 0;
  }
}
#endif

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
      if (loadHIndex==(int)((mouse.y-63+listPos)/20) && loadHIndex<listelem.size()) {
        if (showSampleSel) {
          curSample=loadHIndex;
          showSampleSel=false;
        } else {
          loadSample();
        }
      } else {
        loadHIndex=(!PointInRect(mouse.x,mouse.y,30,60,30+680,60+392))?(-1):((mouse.y-63+listPos)/20);
        if (loadHIndex<listelem.size() && loadHIndex>-1) {
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

void Sampler::seMouseMove(int button) {
  hover(690,10,690+40,10+20,&sloadS);
  hover(630,40,630+40,40+20,&seupS);
  hover(580,40,580+40,40+20,&sedownS);
  
  hover(72-52,124,72-52+40,124+24,&smupS[0]);
  hover(72-52,192,72-52+40,192+24,&smdownS[0]);
  hover(72+52,124,72+52+40,124+24,&smupS[1]);
  hover(72+52,192,72+52+40,192+24,&smdownS[1]);
  hover(257-52,124,257-52+40,124+24,&smupS[2]);
  hover(257-52,192,257-52+40,192+24,&smdownS[2]);
  hover(257+52,124,257+52+40,124+24,&smupS[3]);
  hover(257+52,192,257+52+40,192+24,&smdownS[3]);
  
  hover(10,10,10+60,10+20,&sselectS);
}

void Sampler::seMouseDown(int button) {
  if (PointInRect(mouse.x,mouse.y,690,10,690+40,10+20)) {
    sloadS=2;
  }
  if (PointInRect(mouse.x,mouse.y,10,10,10+60,10+20)) {
    sselectS=2;
  }
  // rate
  if (PointInRect(mouse.x,mouse.y,630,40,630+40,40+20)) {
    seupS=2;
    if (button!=1) {
      doUp=true;
      doXTarget=0;
    } else {
      s[curSample].rate*=2;
    }
  }
  if (PointInRect(mouse.x,mouse.y,580,40,580+40,40+20)) {
    sedownS=2;
    if (button!=1) {
      doDown=true;
      doXTarget=0;
    } else {
      s[curSample].rate/=2;
    }
  }
  /// target ///
  
  // note min
  if (PointInRect(mouse.x,mouse.y,72-52,124,72-52+40,124+24)) {
    smupS[0]=2;
    if (button!=1) {
      doUp=true;
      doXTarget=1;
    } else {
      s[curSample].noteMin+=12;
    }
  }
  if (PointInRect(mouse.x,mouse.y,72-52,192,72-52+40,192+24)) {
    smdownS[0]=2;
    if (button!=1) {
      doDown=true;
      doXTarget=1;
    } else {
      s[curSample].noteMin-=12;
    }
  }
  
  // note max
  if (PointInRect(mouse.x,mouse.y,72+52,124,72+52+40,124+24)) {
    smupS[1]=2;
    if (button!=1) {
      doUp=true;
      doXTarget=2;
    } else {
      s[curSample].noteMax+=12;
    }
  }
  if (PointInRect(mouse.x,mouse.y,72+52,192,72+52+40,192+24)) {
    smdownS[1]=2;
    if (button!=1) {
      doDown=true;
      doXTarget=2;
    } else {
      s[curSample].noteMax-=12;
    }
  }
  
  // vol min
  if (PointInRect(mouse.x,mouse.y,257-52,124,257-52+40,124+24)) {
    smupS[2]=2;
    if (button!=1) {
      doUp=true;
      doXTarget=3;
    } else {
      s[curSample].velMin*=2;
    }
  }
  if (PointInRect(mouse.x,mouse.y,257-52,192,257-52+40,192+24)) {
    smdownS[2]=2;
    if (button!=1) {
      doDown=true;
      doXTarget=3;
    } else {
      s[curSample].velMin/=2;
    }
  }
  
  // vol max
  if (PointInRect(mouse.x,mouse.y,257+52,124,257+52+40,124+24)) {
    smupS[3]=2;
    if (button!=1) {
      doUp=true;
      doXTarget=4;
    } else {
      s[curSample].velMax*=2;
    }
  }
  if (PointInRect(mouse.x,mouse.y,257+52,192,257+52+40,192+24)) {
    smdownS[3]=2;
    if (button!=1) {
      doDown=true;
      doXTarget=4;
    } else {
      s[curSample].velMax/=2;
    }
  }
}

void Sampler::seMouseUp(int button) {
  if (sloadS!=1) {
    sloadS=PointInRect(mouse.x,mouse.y,690,10,690+40,10+20);
    if (sloadS) {
      printf("load?\n");
      readDir(wd.c_str());
      showLoad=true;
      loadHIndex=-1;
    }
  }
  if (sselectS!=1) {
    sselectS=PointInRect(mouse.x,mouse.y,10,10,10+60,10+20);
    if (sselectS) {
      printf("select?\n");
      prepareSampleSel();
      showSampleSel=true;
      loadHIndex=-1;
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
  
  /// range ///
  
  if (smupS[0]!=1) {
    smupS[0]=PointInRect(mouse.x,mouse.y,72-52,124,72-52+40,124+24);
    doUp=false;
  }
  if (smdownS[0]!=1) {
    smdownS[0]=PointInRect(mouse.x,mouse.y,72-52,192,72-52+40,192+24);
    doDown=false;
  }
  
  if (smupS[1]!=1) {
    smupS[1]=PointInRect(mouse.x,mouse.y,72+52,124,72+52+40,124+24);
    doUp=false;
  }
  if (smdownS[1]!=1) {
    smdownS[1]=PointInRect(mouse.x,mouse.y,72+52,192,72+52+40,192+24);
    doDown=false;
  }
  
  if (smupS[2]!=1) {
    smupS[2]=PointInRect(mouse.x,mouse.y,257-52,124,257-52+40,124+24);
    doUp=false;
  }
  if (smdownS[2]!=1) {
    smdownS[2]=PointInRect(mouse.x,mouse.y,257-52,192,257-52+40,192+24);
    doDown=false;
  }
  
  if (smupS[3]!=1) {
    smupS[3]=PointInRect(mouse.x,mouse.y,257+52,124,257+52+40,124+24);
    doUp=false;
  }
  if (smdownS[3]!=1) {
    smdownS[3]=PointInRect(mouse.x,mouse.y,257+52,192,257+52+40,192+24);
    doDown=false;
  }
}

void Sampler::mouseEvent(int type, int button, int x, int y, int finger) {
  switch (type) {
    case 0:
      mouse.x=x; mouse.y=y;
      if (curView==2) {
        seMouseMove(button);
      }
      if (showLoad || showSampleSel) {
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
      if (curView==2) {
        seMouseDown(button);
      }
      if (showLoad || showSampleSel) {
        loadMouseDown(button);
      }
      break;
    case 1: // up
      mouse.b[button]=0;
      if (showLoad || showSampleSel) {
        loadMouseUp(button);
      }
      if (curView==2) {
        seMouseUp(button);
      }
      if (supS!=1) {
        supS=PointInRect(mouse.x,mouse.y,30,30,30+40,30+20);
        if (supS && (showLoad || showSampleSel)) {
          if (showSampleSel) {
            s.resize(s.size()+1);
            int ssize=s.size()-1;
            s[ssize].path="Sample";
            char* sl;
            sl=new char[21];
            sprintf(sl,"%zu",s.size());
            s[ssize].path+=sl;
            delete[] sl;
            s[ssize].noteMin=0;
            s[ssize].noteMax=127;
            s[ssize].velMin=0;
            s[ssize].velMax=127;
            sndf=sf_open("../share/orchestrack/testsmp.wav",SFM_READ,&si);
            s[ssize].len=si.frames;
            s[ssize].chan=si.channels;
            s[ssize].rate=si.samplerate;
            s[ssize].data=new float*[si.channels];
            tbuf=new float[si.channels];
            for (int i=0; i<si.channels; i++) {
              s[ssize].data[i]=new float[si.frames];
            }
            for (int i=0; i<si.frames; i++) {
              sf_readf_float(sndf,tbuf,1);
              for (int j=0; j<si.channels; j++) {
                s[ssize].data[j][i]=tbuf[j];
              }
            }
            sf_close(sndf);
            prepareSampleSel();
          } else {
            printf("goes up\n");
            wd=topLevel(wd);
            readDir(wd.c_str());
            listPos=0;
          }
        }
      }
      if (scancelS!=1) {
        scancelS=PointInRect(mouse.x,mouse.y,660,462,660+50,462+20);
        if (scancelS) {
          printf("cancel\n");
          showLoad=false;
          showSampleSel=false;
        }
      }
      break;
    case 3:
      if (showLoad || showSampleSel) {
        listMouseWheel(x,y);
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
  srange=drawButton(r,0,0,60,36,tempc,4);
  tempc.r=64;
  tempc.g=64;
  tempc.b=64;
  tempc.a=255;
  sload=drawButton(r,0,0,40,20,tempc,4);
  scancel=drawButton(r,0,0,50,20,tempc,4);
  sselect=drawButton(r,0,0,60,20,tempc,4);
  srangebutton=drawButton(r,0,0,40,24,tempc,4);
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

void Sampler::loadSample() {
  if (listings[loadHIndex].type==4) {
    if (wd.at(wd.size()-1)!=DIR_SEP) {
      wd+=DIR_SEP;
    }
    wd+=listings[loadHIndex].name;
    readDir(wd.c_str());
    listPos=0;
    loadHIndex=-1;
    sfname="";
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
      s[curSample].len=si.frames;
      for (int i=0; i<s[curSample].chan; i++) {
        delete[] s[curSample].data[i];
      }
      delete[] s[curSample].data;
      s[curSample].chan=si.channels;
      s[curSample].rate=si.samplerate;
      s[curSample].data=new float*[si.channels];
      tbuf=new float[si.channels];
      for (int i=0; i<si.channels; i++) {
        s[curSample].data[i]=new float[si.frames];
      }
      for (int i=0; i<si.frames; i++) {
        sf_readf_float(sndf,tbuf,1);
        for (int j=0; j<si.channels; j++) {
          s[curSample].data[j][i]=tbuf[j];
        }
      }
      sf_close(sndf);
      s[curSample].path=listings[loadHIndex].name.erase(listings[loadHIndex].name.find_last_of('.'),listings[loadHIndex].name.size()-listings[loadHIndex].name.find_last_of('.'));
      delete[] tbuf;
      showLoad=false;
      busy=false;
    } else {
      sf_perror(sndf);
    }
  }
}

void Sampler::clearList() {
  listelem.resize(0);
}

void Sampler::feedList(string name, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
  listentry tle;
  tle.name=name;
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
  if (loadHIndex!=-1 && loadHIndex<listelem.size()) {
    SDL_SetRenderDrawColor(r,255,255,255,(!scrolling && touching && loadHIndex==(int)((mouse.y-63+listPos)/20))?(128):(64));
    tempr.x=33;
    tempr.y=66+20*loadHIndex-listPos;
    tempr.w=674;
    tempr.h=20;
    SDL_RenderFillRect(r,&tempr);
  }

  for (int i=fmax(0,listPos/20); i<fmin(listelem.size(),20+listPos/20); i++) {
    f->draw(33,66+(20*i)-listPos,listelem[i].color,0,0,0,listelem[i].name);
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
    if ((listPos+382)>20*(listelem.size()) || listPos<0) {
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
    //printf("listPos: %f. target %f\n",listPos,((20*(float)listings.size()-392)+0.5));
  }
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
  
  f->draw(83,30,tempc,0,0,0,wd);
  f->draw(33,462,tempc,0,0,0,sfname);
  //f->draw(33,462,tempc,0,0,0,"filename.wav");
  drawList();
}

void Sampler::drawSampleSel() {
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
  
  f->draw(50,30,tempc,1,0,0,"New");
  f->draw(690,30,tempc,1,0,0,"Del");
  
  f->draw(630,462,tempc,1,0,0,"Select");
  f->draw(685,462,tempc,1,0,0,"Cancel");
  
  f->draw(370,30,tempc,1,0,0,"Select Sample");
  f->draw(33,462,tempc,0,0,0,sfname);
  //f->draw(33,462,tempc,0,0,0,"filename.wav");
  drawList();
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
    tempr.x=v[i].note*5;
    tempr.y=10;
    tempr.w=5;
    tempr.h=340;
    SDL_RenderFillRect(r,&tempr);
  }
  SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(r,64,255,40,64);
  // draw sample regions
  for (int i=0; i<s.size(); i++) {
    tempr.x=10+2+s[i].noteMin*5;
    tempr.y=12;
    tempr.w=5*(s[i].noteMax-s[i].noteMin);
    tempr.h=336;
    SDL_RenderFillRect(r,&tempr);
    SDL_RenderDrawRect(r,&tempr);
  }
  f->draw(370,16,tempc,1,0,0,"Note");
  f->draw(0,200,tempc,1,1,0,"NoteVol");
  f->drawf(83,400,tempc,0,0,"%d %d %d %d",tick/96,(tick/24)%4,(tick/6)%4,tick%6);
}

void Sampler::drawSampleEdit() {
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
  
  tempr.x=10; tempr1.x=60*sselectS;
  tempr.y=10; tempr1.y=0;
  tempr.w=60;  tempr1.w=60;
  tempr.h=20;  tempr1.h=20;
  SDL_RenderCopy(r,sselect,&tempr1,&tempr);
  f->draw(40,10,tempc,1,0,0,"Sample");
  
  f->draw(83,10,tempc,0,0,0,s[curSample].path);
  f->drawf(83,40,tempc,0,0,"%f",s[curSample].rate);
  f->draw(710,10,tempc,1,0,0,"Load");
  f->draw(705,40,tempc,1,0,0,"Keypad");
  f->draw(650,40,tempc,1,0,0,"Up");
  f->draw(600,40,tempc,1,0,0,"Down");
  f->draw(370,70,tempc,1,0,0,"Range");
  f->draw(92,100,tempc,1,0,0,"Note");
  f->draw(277,100,tempc,1,0,0,"NoteVol");
  f->draw(462,100,tempc,1,0,0,"Param1");
  f->draw(647,100,tempc,1,0,0,"Param2");
  f->draw(92,170,tempc,1,1,0,"to");
  f->draw(277,170,tempc,1,1,0,"to");
  f->draw(462,170,tempc,1,1,0,"to");
  f->draw(647,170,tempc,1,1,0,"to");
  
  /// note
  
  // left
  tempr.x=62-52; tempr1.x=0;
  tempr.y=152; tempr1.y=0;
  tempr.w=60;  tempr1.w=60;
  tempr.h=36;  tempr1.h=36;
  SDL_RenderCopy(r,srange,&tempr1,&tempr);
  tempr.x=72-52; tempr1.x=40*smupS[0];
  tempr.y=124; tempr1.y=0;
  tempr.w=40;  tempr1.w=40;
  tempr.h=24;  tempr1.h=24;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  tempr.x=72-52; tempr1.x=40*smdownS[0];
  tempr.y=192; tempr1.y=0;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  f->draw(92-52,126,tempc,1,0,0,"Up");
  f->draw(92-52,194,tempc,1,0,0,"Down");
  f->drawf(92-52,170,tempc,1,1,
           "%c%c%d",sChromaNote[s[curSample].noteMin%12]
                   ,sChromaSemitone[s[curSample].noteMin%12]
                   ,s[curSample].noteMin/12);
  
  // right
  tempr.x=62+52; tempr1.x=0;
  tempr.y=152; tempr1.y=0;
  tempr.w=60;  tempr1.w=60;
  tempr.h=36;  tempr1.h=36;
  SDL_RenderCopy(r,srange,&tempr1,&tempr);
  tempr.x=72+52; tempr1.x=40*smupS[1];
  tempr.y=124; tempr1.y=0;
  tempr.w=40;  tempr1.w=40;
  tempr.h=24;  tempr1.h=24;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  tempr.x=72+52; tempr1.x=40*smdownS[1];
  tempr.y=192; tempr1.y=0;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  f->draw(92+52,126,tempc,1,0,0,"Up");
  f->draw(92+52,194,tempc,1,0,0,"Down");
  f->drawf(92+52,170,tempc,1,1,
           "%c%c%d",sChromaNote[s[curSample].noteMax%12]
                   ,sChromaSemitone[s[curSample].noteMax%12]
                   ,s[curSample].noteMax/12);
  
  /// notevol
  
  // left
  tempr.x=247-52; tempr1.x=0;
  tempr.y=152; tempr1.y=0;
  tempr.w=60;  tempr1.w=60;
  tempr.h=36;  tempr1.h=36;
  SDL_RenderCopy(r,srange,&tempr1,&tempr);
  tempr.x=257-52; tempr1.x=40*smupS[2];
  tempr.y=124; tempr1.y=0;
  tempr.w=40;  tempr1.w=40;
  tempr.h=24;  tempr1.h=24;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  tempr.x=257-52; tempr1.x=40*smdownS[2];
  tempr.y=192; tempr1.y=0;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  f->draw(277-52,126,tempc,1,0,0,"Up");
  f->draw(277-52,194,tempc,1,0,0,"Down");
  f->drawf(277-52,170,tempc,1,1,
           "%d",s[curSample].velMin);
  
  // right
  tempr.x=247+52; tempr1.x=0;
  tempr.y=152; tempr1.y=0;
  tempr.w=60;  tempr1.w=60;
  tempr.h=36;  tempr1.h=36;
  SDL_RenderCopy(r,srange,&tempr1,&tempr);
  tempr.x=257+52; tempr1.x=40*smupS[3];
  tempr.y=124; tempr1.y=0;
  tempr.w=40;  tempr1.w=40;
  tempr.h=24;  tempr1.h=24;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  tempr.x=257+52; tempr1.x=40*smdownS[3];
  tempr.y=192; tempr1.y=0;
  SDL_RenderCopy(r,srangebutton,&tempr1,&tempr);
  f->draw(277+52,126,tempc,1,0,0,"Up");
  f->draw(277+52,194,tempc,1,0,0,"Down");
  f->drawf(277+52,170,tempc,1,1,
           "%d",s[curSample].velMax);
  
  if (doUp) {
    if (timeOnButton%(int)fmax(64-timeOnButton,1)==0) {
      switch (doXTarget) {
        case 0:
          s[curSample].rate+=(int)fmax(1,pow(10,(float)timeOnButton/128)/10);
          break;
        case 1:
          s[curSample].noteMin+=(int)fmax(1,pow(10,(float)timeOnButton/128)/10);
          break;
        case 2:
          s[curSample].noteMax+=(int)fmax(1,pow(10,(float)timeOnButton/128)/10);
          break;
        case 3:
          s[curSample].velMin+=(int)fmax(1,pow(10,(float)timeOnButton/128)/10);
          break;
        case 4:
          s[curSample].velMax+=(int)fmax(1,pow(10,(float)timeOnButton/128)/10);
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
          s[curSample].noteMin-=(int)fmax(1,pow(10,(float)timeOnButton/128)/10);
          break;
        case 2:
          s[curSample].noteMax-=(int)fmax(1,pow(10,(float)timeOnButton/128)/10);
          break;
        case 3:
          s[curSample].velMin-=(int)fmax(1,pow(10,(float)timeOnButton/128)/10);
          break;
        case 4:
          s[curSample].velMax-=(int)fmax(1,pow(10,(float)timeOnButton/128)/10);
          break;
      }
    }
    s[curSample].rate=fmax(0,s[curSample].rate);
    s[curSample].noteMin=fmax(0,s[curSample].noteMin);
    s[curSample].noteMax=fmax(0,s[curSample].noteMax);
    s[curSample].velMin=fmax(0,s[curSample].velMin);
    s[curSample].velMax=fmax(0,s[curSample].velMax);
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
  
  if (showSampleSel) {
    drawSampleSel();
  }
  
  /***
  SDL_SetRenderDrawColor(r,(mouse.b[0])?(0):(255),(mouse.b[1])?(0):(255),(mouse.b[2])?(0):(255),255);
  SDL_RenderDrawLine(r,mouse.x,mouse.y,0,0);
  ***/
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
    s[0].path="Sample1";
    s[0].noteMin=0;
    s[0].noteMax=127;
    s[0].velMin=0;
    s[0].velMax=127;
    sndf=sf_open("../share/orchestrack/testsmp.wav",SFM_READ,&si);
    s[0].len=si.frames;
    s[0].chan=si.channels;
    s[0].rate=si.samplerate;
    s[0].data=new float*[si.channels];
    tbuf=new float[si.channels];
    for (int i=0; i<si.channels; i++) {
      s[0].data[i]=new float[si.frames];
    }
    for (int i=0; i<si.frames; i++) {
      sf_readf_float(sndf,tbuf,1);
      for (int j=0; j<si.channels; j++) {
        s[0].data[j][i]=tbuf[j];
      }
    }
    sf_close(sndf);
    delete[] tbuf;
    showHidden=false;
    busy=false;
    return true;
  } else {
    return false;
  }
}
