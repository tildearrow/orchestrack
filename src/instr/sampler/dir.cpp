#include "sampler.h"

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
            dede.size=de->nFileSizeLow;//+((de->nFileSizeHigh)<<32);
          }
        }
      }
    }
    FindClose(od);
    delete de;
    clearList();
    for (size_t i=0; i<listings.size(); i++) {
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
      int suffix=0;
      while (listings[i].size>=1024) {
        listings[i].size>>=10;
	suffix++;
      }
      char* sizee;
      sizee=new char[21];
      sprintf(sizee,"%d%c",listings[i].size,fSuffix[suffix]);
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
