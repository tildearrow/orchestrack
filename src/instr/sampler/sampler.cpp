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

inline float Sampler::intNone(float* b, int n, float d) {
  return b[n];
}

inline float Sampler::intLinear(float* b, int n, float d) {
  return b[n]+((b[n+1]-b[n])*d);
}

// slightly modified version of http://www.musicdsp.org/showone.php?id=49
inline float Sampler::intCubic(float* b, int n, float d) {
  float a, bb, c;
  a=(3*(b[n]-b[n+1])-b[n-1]+b[n+2])/2;
  bb=2*b[n+1]+b[n-1]-(5*b[n]+b[n+2])/2;
  c=(b[n+1]-b[n-1])/2;
  return (((a*d)+bb)*d+c)*d+b[n];
}

inline float Sampler::intSinc(float* b, int n, float d) {
  float one;
  one=0;
  int pl;
  float* where;
  pl=((int)(d*65536))&0xffff;
  where=table+(((pl+fHalve)>>fShift)&fMask);
  for (int i=-3; i<5; i++) {
    one+=b[n+i]*where[i+3];
  }
  return one;
}

float* Sampler::getSample() {
  int i, j;
  if (busy) {v.resize(0); return sample;}
  float element;
  sample[0]=0;
  sample[1]=0;
  ev=(unsigned char*)getEvent();
  while (ev!=NULL) {
    if ((ev[0]>>4)==8) {
      // find voice with properties, then destroy it
      for (i=0; i<v.size(); i++) {
        if (v[i].chan==(ev[0]&15)) {
          if (v[i].note==ev[1]) {
            //printf("erased. %d %.2x %.2x\n",i,v[i].chan,v[i].note);
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
      v[thisv].periodN=0;
      v[thisv].periodD=0;
      v[thisv].sample=0;
      for (i=0; i<s.size(); i++) {
        if (s[i].noteMin<=v[thisv].note && s[i].noteMax>=v[thisv].note &&
            s[i].velMin<=ev[2] && s[i].velMax>=ev[2]) {
          v[thisv].sample=i; break;
        }
      }
      v[thisv].f=pow(2.0,((float)v[thisv].note-60.0)/12.0)*s[v[thisv].sample].rate/44100.0;
      v[thisv].vol=(float)ev[2]/128.0;
      //printf("allocated. %d %.2x %.2x %f %f\n",thisv,v[thisv].chan,v[thisv].note,v[thisv].vol,v[thisv].f);
    }
    if ((ev[0]>>4)==0xe) {
      // pitch bend.
      c[ev[0]&15].pitch=(ev[1]+(ev[2]<<7))-0x2000;
      //printf("pitch %d\n",c[ev[0]&15].pitch);
      for (i=0; i<v.size(); i++) {
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
  for (i=0; i<v.size(); i++) {
    if (s[v[i].sample].chan==1) {
      /*
      element0=s[v[i].sample].data[0][v[i].periodN];
      element1=s[v[i].sample].data[0][v[i].periodN+1];
      element=element0+((element1-element0)*v[i].periodD);*/
      
      element=intSinc(s[v[i].sample].data[0],v[i].periodN+8,v[i].periodD);
      
      sample[0]+=element*v[i].vol;
      sample[1]+=element*v[i].vol;
    } else for (j=0; j<s[v[i].sample].chan; j++) {
      /*
      element0=s[v[i].sample].data[j][v[i].periodN];
      element1=s[v[i].sample].data[j][v[i].periodN+1];*/
      element=intSinc(s[v[i].sample].data[j],v[i].periodN+8,v[i].periodD);
      
      sample[j]+=element*v[i].vol;
    }
    v[i].periodD+=v[i].f;
    v[i].periodN+=(int)v[i].periodD;
    if (s[v[i].sample].loopType==1 && v[i].periodN>s[v[i].sample].loopEnd) {
      v[i].periodN=s[v[i].sample].loopStart+(v[i].periodN%(s[v[i].sample].loopEnd+1));
    }
    v[i].periodD=fmod(v[i].periodD,1);
  }
  
  for (i=0; i<v.size(); i++) {
    if ((int)v[i].periodN>s[v[i].sample].len) {
      //printf("sample finished. %d %f %d\n",i,v[i].period,s[v[i].sample].len);
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
    feedList(s[i].path,"",255,255,255,255);
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

string Sampler::friendlyErr0(int e) {
  switch (e) {
    case 0: return "The operation was a success!";
    case 1: return "You're not able to do that.";
    case 2: return "That file/directory doesn't exist.";
    case 5: return "There was a I/O error!";
    case 6: return "That device doesn't exist.";
    case 7: return "You specified too many arguments.";
    case 12: return "Can't allocate memory!";
    case 13: return "You don't have permission.";
    case 16: return "That device is busy.";
    case 17: return "That file already exists.";
    case 19: return "That device doesn't exist.";
    case 20: return "That is not a directory.";
    case 21: return "That is a directory.";
    case 22: return "You have provided an invalid argument.";
    case 23: return "Too many files are open!";
    case 24: return "You have opened too many files!";
    case 27: return "That file is too large.";
    case 28: return "You don't have more device space!";
    case 30: return "That file system is read-only.";
    case 36: return "That file name is too long.";
    case 95: return "You can't do that.";
    case 121: return "There was a remote I/O error!";
    case 122: return "You have exceeded your disk quota!";
    case 256: return "That format can't be recognized.";
    case 257: return "That file is foul.";
    case 258: return "That file has an unsupported encoding.";
    default: return "An error has occured:";
  }
}

string Sampler::friendlyErr1(int e) {
  switch (e) {
    case 0: return "This couldn't happen though...";
    case 1: return "Try contacting your system administrator";
    case 2: return "Maybe you made a typo, or that file truly";
    case 5: return "Your device might be corrupted...";
    case 6: return "Specify a different device, and";
    case 7: return "Specify less, and try";
    case 12: return "Try closing some apps.";
    case 13: return "Try contacting your system administrator";
    case 16: return "Wait a little, then";
    case 17: return "Please use a different file";
    case 19: return "Try with a different";
    case 20: return "Specify a directory, then";
    case 21: return "Specify a non-directory, then";
    case 22: return "Maybe you made a typo.";
    case 23: return "Increase the system limits, then";
    case 24: return "Increase your limits, then";
    case 27: return "Try with a smaller";
    case 28: return "Make up some space, then";
    case 30: return "Remount read-write if possible, then";
    case 36: return "Shorten it please.";
    case 95: return "Try doing something else.";
    case 121: return "Contact that host's administrator.";
    case 122: return "Try contacting your system administrator";
    case 256: return "If it's a sound file, convert that file to";
    case 257: return "I don't think I can do anything";
    case 258: return "Convert that file to a different format,";
    default: return strerror(e);
  }
}

string Sampler::friendlyErr2(int e) {
  switch (e) {
    case 0: return "Report a bug at https://github.com/tildearrow/orchestrack";
    case 1: return "(if you are him/her, then I'm sorry).";
    case 2: return "doesn't exist.";
    case 5: return "Replace it as soon as possible!";
    case 6: return "try again.";
    case 7: return "again.";
    case 12: return "Then try again.";
    case 13: return "(if you are him/her, give yourself permission to that file).";
    case 16: return "try again.";
    case 17: return "or delete it.";
    case 19: return "device.";
    case 20: return "try again.";
    case 21: return "try again.";
    case 22: return "";
    case 23: return "try again.";
    case 24: return "try again.";
    case 27: return "file.";
    case 28: return "try again.";
    case 30: return "try again.";
    case 36: return "";
    case 95: return "";
    case 121: return "";
    case 122: return "(if you are him/her, increase your limits).";
    case 256: return "a supported format first.";
    case 257: return "to help you...";
    case 258: return "then try again.";
    default: return "";
  }
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
	    dede.size=de->nFileSizeLow+((de->nFileSizeHigh)<<32);
          }
        }
      }
    }
    FindClose(od);
    delete de;
    clearList();
    for (int i=0; i<listings.size(); i++) {
      char* sizee;
      sizee=new char[21];
      sprintf(sizee,"%d",listings[i].size);
      switch (listings[i].type) {
        case 1: feedList(listings[i].name,"<fifo>",255,192,160,255); break; // fifo
        case 2: feedList(listings[i].name,"<char>",255,255,160,255); break; // character
        case 4: feedList(listings[i].name,"<dir>",160,192,255,255); break; // directory
        case 6: feedList(listings[i].name,"<blk>",255,220,160,255); break; // block
        case 8: feedList(listings[i].name,sizee,255,255,255,255); break; // file
        case 10: feedList(listings[i].name,"<link>",160,220,255,255); break; // link
        case 12: feedList(listings[i].name,"<sock>",255,128,255,255); break; // socket
        default: feedList(listings[i].name,"<?>",128,128,128,255); break; // unknown
      }
      delete[] sizee;
    }
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
  struct stat st;
  od=opendir(path);
  dentry tempelem;
  string tempstr;
  if (od!=NULL) {
    listings.clear();
    while (1) {
      de=readdir(od);
      if (!de) {
        break;
      } else {
        if (strcmp(".",de->d_name)!=0 && strcmp("..",de->d_name)!=0) {
          if (showHidden || (de->d_name[0]!='.')) {
            tempstr=path;
            tempstr+='/';
            tempstr+=de->d_name;
            stat(tempstr.c_str(),&st);
            dede.name=de->d_name;
            dede.type=de->d_type;
            dede.size=st.st_size;
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
      char* sizee;
      sizee=new char[21];
      sprintf(sizee,"%d",listings[i].size);
      switch (listings[i].type) {
        case 1: feedList(listings[i].name,"<fifo>",255,192,160,255); break; // fifo
        case 2: feedList(listings[i].name,"<char>",255,255,160,255); break; // character
        case 4: feedList(listings[i].name,"<dir>",160,192,255,255); break; // directory
        case 6: feedList(listings[i].name,"<blk>",255,220,160,255); break; // block
        case 8: feedList(listings[i].name,sizee,255,255,255,255); break; // file
        case 10: feedList(listings[i].name,"<link>",160,220,255,255); break; // link
        case 12: feedList(listings[i].name,"<sock>",255,128,255,255); break; // socket
        default: feedList(listings[i].name,"<?>",128,128,128,255); break; // unknown
      }
      delete[] sizee;
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

void Sampler::seMouseMove(int button) {
  if (showLoad || showSampleSel) {
    return;
  }
  hover(690,10,690+40,10+20,&sloadS);
  hover(630,40,630+40,40+20,&seupS);
  hover(580,40,580+40,40+20,&sedownS);
  
  hover(10,10,10+60,10+20,&sselectS);
}

void Sampler::seMouseDown(int button) {
  if (showLoad || showSampleSel) {
    return;
  }
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
}

void Sampler::seMouseUp(int button) {
  if (showLoad || showSampleSel) {
    return;
  }
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
}

void Sampler::mouseEvent(int type, int button, int x, int y, int finger) {
  switch (type) {
    case 0:
      mouse.x=x; mouse.y=y;
      if (curView==1) {
        grMouseMove(button);
      }
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
      if (curView==1) {
        grMouseDown(button);
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
      if (curView==1) {
        grMouseUp(button);
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
              s[ssize].data[i]=new float[si.frames+16]; // prevent click
              for (int j=0; j<si.frames+16; j++) {
                s[ssize].data[i][j]=0;
              }
            }
            for (int i=8; i<si.frames+8; i++) {
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
      wdoff=fmax(0,fmin(sx-573,wdoff));
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
        s[curSample].data[i]=new float[si.frames+16];
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
      s[curSample].path=listings[loadHIndex].name.erase(listings[loadHIndex].name.find_last_of('.'),listings[loadHIndex].name.size()-listings[loadHIndex].name.find_last_of('.'));
      delete[] tbuf;
      FILE* fo;
      fo=fopen(path.c_str(),"rb");
      freeRIFF(lf);
      lf=readRIFF(fo);
      fclose(fo);
      freeWAVE(lwf);
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
        default: fprintf(stderr,"huh?\n");
      }
      erra=64;
      errl[0]=friendlyErr0(ee);
      errl[1]=friendlyErr1(ee);
      errl[2]=friendlyErr2(ee);
    }
  }
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
    f->draw(33+671,66+(20*i)-listPos,listelem[i].color,2,0,0,listelem[i].rh);
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
  f->draw(83-fmax(0,fmin(sx-573,wdoff)),30,tempc,0,0,0,wd);
  SDL_RenderSetClipRect(r,NULL);
  f->draw(33,462,tempc,0,0,0,sfname);
  //f->draw(33,462,tempc,0,0,0,"filename.wav");
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

void Sampler::upDown() {
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
  
  upDown();
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
    FILE* fo;
    fo=fopen("../share/orchestrack/testsmp.wav","rb");
    lf=readRIFF(fo);
    fclose(fo);
    lwf=readWAVE(lf);
    sndf=sf_open("../share/orchestrack/testsmp.wav",SFM_READ,&si);
    s[0].len=si.frames;
    s[0].chan=si.channels;
    s[0].rate=si.samplerate;
    s[0].data=new float*[si.channels];
    tbuf=new float[si.channels];
    for (int i=0; i<si.channels; i++) {
      s[0].data[i]=new float[si.frames+16];
      for (int j=0; j<si.frames+16; j++) {
        s[0].data[i][j]=0;
      }
    }
    for (int i=8; i<si.frames+8; i++) {
      sf_readf_float(sndf,tbuf,1);
      for (int j=0; j<si.channels; j++) {
        s[0].data[j][i]=tbuf[j];
      }
    }
    sf_close(sndf);
    delete[] tbuf;
    windowed_fir_init(table);
    showHidden=false;
    busy=false;
    return true;
  } else {
    return false;
  }
}
