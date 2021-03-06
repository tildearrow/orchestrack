#include "button.h"

int roundRect(unsigned char* ptr, int tw, SDL_Color c, SDL_Color c1, int x, int y, int w, int h, int rr) {
  float res[257]; // 16x sampling, period.
  unsigned char* bitmap;
  int where;
  // prepare bitmap
  bitmap=new unsigned char[w*h*4];
  memset(bitmap,0,w*h*4);
  
  // fill
  for (int i=0; i<w; i++) {
    for (int j=rr; j<h-rr; j++) {
      bitmap[(i*4)+(j*w*4)]=255;
      bitmap[(i*4)+(j*w*4)+1]=255;
      bitmap[(i*4)+(j*w*4)+2]=255;
      bitmap[(i*4)+(j*w*4)+3]=255;
    }
  }
  for (int i=0+rr; i<w-rr; i++) {
    for (int j=0; j<rr; j++) {
      bitmap[(i*4)+(j*w*4)]=255;
      bitmap[(i*4)+(j*w*4)+1]=255;
      bitmap[(i*4)+(j*w*4)+2]=255;
      bitmap[(i*4)+(j*w*4)+3]=255;
    }
    for (int j=h-rr; j<h; j++) {
      bitmap[(i*4)+(j*w*4)]=255;
      bitmap[(i*4)+(j*w*4)+1]=255;
      bitmap[(i*4)+(j*w*4)+2]=255;
      bitmap[(i*4)+(j*w*4)+3]=255;
    }
  }
  
  // smooth sides
  for (int i=rr; i<w-rr; i++) {
    bitmap[(i*4)]=255;
    bitmap[(i*4)+1]=255;
    bitmap[(i*4)+2]=255;
    bitmap[(i*4)+3]=255;
    bitmap[((h-1)*w*4)+(i*4)]=255;
    bitmap[((h-1)*w*4)+(i*4)+1]=255;
    bitmap[((h-1)*w*4)+(i*4)+2]=255;
    bitmap[((h-1)*w*4)+(i*4)+3]=255;
  }
  for (int i=rr; i<h-rr; i++) {
    bitmap[((i)*w*4)]=255;
    bitmap[((i)*w*4)+1]=255;
    bitmap[((i)*w*4)+2]=255;
    bitmap[((i)*w*4)+3]=255;
    bitmap[((w-1)*4)+((i)*w*4)]=255;
    bitmap[((w-1)*4)+((i)*w*4)+1]=255;
    bitmap[((w-1)*4)+((i)*w*4)+2]=255;
    bitmap[((w-1)*4)+((i)*w*4)+3]=255;
  }
  
  // corners
  for (int i=0; i<rr; i++) {
    for (int j=0; j<rr; j++) {
      res[0]=0;
      for (int k=0; k<16; k++) {
        for (int l=0; l<16; l++) {
          if (pow(pow((float)rr-(float)i-((float)k/16.0),2.0)+pow((float)rr-(float)j-((float)l/16.0),2.0),0.5)<(rr)) {
            res[0]++;
          }
        }
      }
      /*
      res[0]=pow(pow(rr*2-i*2,2)+pow(rr*2-j*2,2),0.5)<(rr*2);
      res[1]=pow(pow(rr*2-i*2-1,2)+pow(rr*2-j*2,2),0.5)<(rr*2);
      res[2]=pow(pow(rr*2-i*2-1,2)+pow(rr*2-j*2-1,2),0.5)<(rr*2);
      res[3]=pow(pow(rr*2-i*2,2)+pow(rr*2-j*2-1,2),0.5)<(rr*2);
      res[4]=(res[0]+res[1]+res[2]+res[3])/4;*/
      
      for (int k=0; k<4; k++) {
        switch (k) {
          case 0: where=((i)*4)+((j)*w*4); break;
          case 1: where=((w-1-i)*4)+((j)*w*4); break;
          case 2: where=((i)*4)+((h-1-j)*w*4); break;
          case 3: where=((w-1-i)*4)+((h-1-j)*w*4); break;
        }
        bitmap[where]=255;
        bitmap[where+1]=255;
        bitmap[where+2]=255;
        bitmap[where+3]=(unsigned char)(fmin(255,res[0]));
      }
    }
  }
  
  // apply color/gradient
  for (int i=0; i<w; i++) {
    for (int j=0; j<h; j++) {
      where=(i*4)+(j*w*4);
      bitmap[where]=(unsigned char)((float)c.b+(((float)c1.b-(float)c.b)*(float)j/(float)h));
      bitmap[where+1]=(unsigned char)((float)c.g+(((float)c1.g-(float)c.g)*(float)j/(float)h));
      bitmap[where+2]=(unsigned char)((float)c.r+(((float)c1.r-(float)c.r)*(float)j/(float)h));
      bitmap[where+3]=(unsigned char)((float)bitmap[where+3]*(((float)c.a+(((float)c1.a-(float)c.a)*(float)j/(float)h))/255.0));
    }
  }
  
  for (int i=x; i<x+w; i++) {
    for (int j=y; j<y+h; j++) {
      int src, dest;
      src=((i-x)*4)+((j-y)*w*4);
      dest=(i*4)+(j*tw*4);
      ptr[dest]=(unsigned char)(255.0*(((float)bitmap[src]/255.0)*((float)bitmap[src+3]/255.0)+(((float)ptr[dest]/255.0)*(1.0-((float)bitmap[src+3]/255.0)))));
      ptr[dest+1]=(unsigned char)(255.0*(((float)bitmap[src+1]/255.0)*((float)bitmap[src+3]/255.0)+(((float)ptr[dest+1]/255.0)*(1.0-((float)bitmap[src+3]/255.0)))));
      ptr[dest+2]=(unsigned char)(255.0*(((float)bitmap[src+2]/255.0)*((float)bitmap[src+3]/255.0)+(((float)ptr[dest+2]/255.0)*(1.0-((float)bitmap[src+3]/255.0)))));
      ptr[dest+3]=(unsigned char)(255.0*(((float)bitmap[src+3]/255.0)+(((float)ptr[dest+3]/255.0)*(1.0-((float)bitmap[src+3]/255.0)))));
    }
  }
  
  delete[] bitmap;

  return 0;
}

SDL_Texture* drawButton(SDL_Renderer* r, int x, int y, int w, int h, SDL_Color color, int rr) {
  SDL_Rect rect;
  FILE* cl;
  FILE* clw;
  char* cn;
#ifdef _WIN32
  cn=new char[MAX_PATH];
#else
  cn=new char[PATH_MAX];
#endif
  sprintf(cn,"bcache-%x-%x-%x-%x%x%x%x",w,h,rr,color.r,color.g,color.b,color.a);
  rect.x=0; rect.y=0; rect.w=w; rect.h=h;
  SDL_Texture* t;
  t=SDL_CreateTexture(r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,w*3,h);
  SDL_SetTextureBlendMode(t,SDL_BLENDMODE_BLEND);
  void* p=NULL;
  int pitch;
  if (!SDL_LockTexture(t,NULL,&p,&pitch)) {
    unsigned char* pp;
    SDL_Color tempcolor;
    SDL_Color tempcolor1;
    pp=new unsigned char[w*3*h*4];//(unsigned char*)p;
    
    unsigned char* ppp;
    ppp=(unsigned char*)p;
    
    cl=fopen(cn,"rb");
    if (cl) {
      fread(ppp,1,w*3*h*4,cl);
      fclose(cl);
    } else {
    
    memset(pp,0,w*3*h*4);

    // --- first --- //
    
    tempcolor.r=(unsigned char)fmin(255,color.r+128);
    tempcolor.g=(unsigned char)fmin(255,color.g+128);
    tempcolor.b=(unsigned char)fmin(255,color.b+128);
    tempcolor.a=255;
    tempcolor1.r=color.r;
    tempcolor1.g=color.g;
    tempcolor1.b=color.b;
    tempcolor1.a=255;
    roundRect(pp,w*3,tempcolor,tempcolor1,0,0,w,h,rr);

    tempcolor.r=(unsigned char)((float)color.r*0.7);
    tempcolor.g=(unsigned char)((float)color.g*0.7);
    tempcolor.b=(unsigned char)((float)color.b*0.7);
    tempcolor.a=255;
    tempcolor1.r=(unsigned char)((float)color.r*0.6);
    tempcolor1.g=(unsigned char)((float)color.g*0.6);
    tempcolor1.b=(unsigned char)((float)color.b*0.6);
    tempcolor1.a=255;
    roundRect(pp,w*3,tempcolor,tempcolor1,1,1,w-2,h-2,rr-2);
    
    // glow
    tempcolor.r=255;
    tempcolor.g=255;
    tempcolor.b=255;
    tempcolor.a=(unsigned char)(128.0*(((float)color.r+(float)color.g+(float)color.b)/510.0));
    tempcolor1.r=255;
    tempcolor1.g=255;
    tempcolor1.b=255;
    tempcolor1.a=0;
    roundRect(pp,w*3,tempcolor,tempcolor1,2,2,w-4,h-12,rr-2);
    
    // --- second --- //
    
    tempcolor.r=(unsigned char)fmin(255,color.r+128);
    tempcolor.g=(unsigned char)fmin(255,color.g+128);
    tempcolor.b=(unsigned char)fmin(255,color.b+128);
    tempcolor.a=255;
    tempcolor1.r=color.r;
    tempcolor1.g=color.g;
    tempcolor1.b=color.b;
    tempcolor1.a=255;
    roundRect(pp,w*3,tempcolor,tempcolor1,w,0,w,h,rr);

    tempcolor.r=(unsigned char)((float)color.r*0.8);
    tempcolor.g=(unsigned char)((float)color.g*0.8);
    tempcolor.b=(unsigned char)((float)color.b*0.8);
    tempcolor.a=255;
    tempcolor1.r=(unsigned char)((float)color.r*0.6);
    tempcolor1.g=(unsigned char)((float)color.g*0.6);
    tempcolor1.b=(unsigned char)((float)color.b*0.6);
    tempcolor1.a=255;
    roundRect(pp,w*3,tempcolor,tempcolor1,w+1,1,w-2,h-2,rr-2);
    
    // glow
    tempcolor.r=255;
    tempcolor.g=255;
    tempcolor.b=255;
    tempcolor.a=(unsigned char)(128.0*(((float)color.r+(float)color.g+(float)color.b)/400.0));
    tempcolor1.r=255;
    tempcolor1.g=255;
    tempcolor1.b=255;
    tempcolor1.a=0;
    roundRect(pp,w*3,tempcolor,tempcolor1,w+2,2,w-4,h-12,rr-2);
    
    // --- third --- //
    
    tempcolor.r=(unsigned char)fmin(255,color.r+128);
    tempcolor.g=(unsigned char)fmin(255,color.g+128);
    tempcolor.b=(unsigned char)fmin(255,color.b+128);
    tempcolor.a=255;
    tempcolor1.r=color.r;
    tempcolor1.g=color.g;
    tempcolor1.b=color.b;
    tempcolor1.a=255;
    roundRect(pp,w*3,tempcolor,tempcolor1,w*2,0,w,h,rr);

    tempcolor.r=(unsigned char)((float)color.r*0.5);
    tempcolor.g=(unsigned char)((float)color.g*0.5);
    tempcolor.b=(unsigned char)((float)color.b*0.5);
    tempcolor.a=255;
    tempcolor1.r=(unsigned char)((float)color.r*0.8);
    tempcolor1.g=(unsigned char)((float)color.g*0.8);
    tempcolor1.b=(unsigned char)((float)color.b*0.8);
    tempcolor1.a=255;
    roundRect(pp,w*3,tempcolor,tempcolor1,(w*2)+1,1,w-2,h-2,rr-2);
    
    // glow
    tempcolor.r=255;
    tempcolor.g=255;
    tempcolor.b=255;
    tempcolor.a=(unsigned char)(128.0*(((float)color.r+(float)color.g+(float)color.b)/510.0));
    tempcolor1.r=255;
    tempcolor1.g=255;
    tempcolor1.b=255;
    tempcolor1.a=0;
    roundRect(pp,w*3,tempcolor,tempcolor1,(w*2)+3,3,w-6,h-10,rr-2);
    
    // cache write //
    clw=fopen(cn,"wb");
    if (clw) {
      for (int i=0; i<w*3*h*4; i++) {
        fputc(pp[i],clw);
      }
      fclose(clw);
    } else {
      printf("can't cache button.\n");
    }
    // copy result
    for (int i=0; i<w*3; i++) {
      for (int j=0; j<h; j++) {
        ppp[(i*4)+(j*pitch)]=pp[(i*4)+(j*pitch)];//color.b*pp[i*4]/255;
        ppp[(i*4)+(j*pitch)+1]=pp[(i*4)+(j*pitch)+1];//color.g*pp[i*4+1]/255;
        ppp[(i*4)+(j*pitch)+2]=pp[(i*4)+(j*pitch)+2];//color.r*pp[i*4+2]/255;
        ppp[(i*4)+(j*pitch)+3]=pp[(i*4)+(j*pitch)+3];//color.a;
      }
    }
    }
    
    delete[] pp;
    
    SDL_UnlockTexture(t);
  } else {
    fprintf(stderr,"can't.\n");
  }
  rect.x=x; rect.y=y;
  
  delete[] cn;
  return t;
}
