#include "button.h"

int roundRect(unsigned char* ptr, int tw, SDL_Color c, int x, int y, int w, int h, int rr) {
  float res[5]; // 2x sampling
  int where;
  // fill
  for (int i=x; i<x+w; i++) {
    for (int j=y+rr; j<y+h-rr; j++) {
      ptr[(i*4)+(j*tw*4)]=c.b;
      ptr[(i*4)+(j*tw*4)+1]=c.g;
      ptr[(i*4)+(j*tw*4)+2]=c.r;
      ptr[(i*4)+(j*tw*4)+3]=c.a;
    }
  }
  for (int i=x+rr; i<x+w-rr; i++) {
    for (int j=y; j<y+h; j++) {
      ptr[(i*4)+(j*tw*4)]=c.b;
      ptr[(i*4)+(j*tw*4)+1]=c.g;
      ptr[(i*4)+(j*tw*4)+2]=c.r;
      ptr[(i*4)+(j*tw*4)+3]=c.a;
    }
  }
  
  // smooth sides
  for (int i=x+rr; i<x+w-rr; i++) {
    ptr[(i*4)+(y*tw*4)]=(unsigned char)((float)c.b*0.75);
    ptr[(i*4)+(y*tw*4)+1]=(unsigned char)((float)c.g*0.75);
    ptr[(i*4)+(y*tw*4)+2]=(unsigned char)((float)c.r*0.75);
    ptr[(i*4)+(y*tw*4)+3]=c.a;
  }
  for (int i=x+rr; i<x+w-rr; i++) {
    ptr[((y+h-1)*tw*4)+(i*4)]=(unsigned char)((float)c.b*0.75);
    ptr[((y+h-1)*tw*4)+(i*4)+1]=(unsigned char)((float)c.g*0.75);
    ptr[((y+h-1)*tw*4)+(i*4)+2]=(unsigned char)((float)c.r*0.75);
    ptr[((y+h-1)*tw*4)+(i*4)+3]=c.a;
  }
  for (int i=rr; i<h-rr; i++) {
    ptr[(x*4)+((y+i)*tw*4)]=(unsigned char)((float)c.b*0.75);
    ptr[(x*4)+((y+i)*tw*4)+1]=(unsigned char)((float)c.g*0.75);
    ptr[(x*4)+((y+i)*tw*4)+2]=(unsigned char)((float)c.r*0.75);
    ptr[(x*4)+((y+i)*tw*4)+3]=c.a;
  }
  for (int i=rr; i<h-rr; i++) {
    ptr[((x+w-1)*4)+((y+i)*tw*4)]=(unsigned char)((float)c.b*0.75);
    ptr[((x+w-1)*4)+((y+i)*tw*4)+1]=(unsigned char)((float)c.g*0.75);
    ptr[((x+w-1)*4)+((y+i)*tw*4)+2]=(unsigned char)((float)c.r*0.75);
    ptr[((x+w-1)*4)+((y+i)*tw*4)+3]=c.a;
  }
  // corners
  for (int i=0; i<rr; i++) {
    for (int j=0; j<rr; j++) {
      res[0]=pow(pow(rr*2-i*2,2)+pow(rr*2-j*2,2),0.5)<(rr*2);
      res[1]=pow(pow(rr*2-i*2-1,2)+pow(rr*2-j*2,2),0.5)<(rr*2);
      res[2]=pow(pow(rr*2-i*2-1,2)+pow(rr*2-j*2-1,2),0.5)<(rr*2);
      res[3]=pow(pow(rr*2-i*2,2)+pow(rr*2-j*2-1,2),0.5)<(rr*2);
      res[4]=(res[0]+res[1]+res[2]+res[3])/4;
      
      for (int k=0; k<4; k++) {
        switch (k) {
          case 0: where=((i+x)*4)+((j+y)*tw*4); break;
          case 1: where=((w-1-i+x)*4)+((j+y)*tw*4); break;
          case 2: where=((i+x)*4)+((h-1-j+y)*tw*4); break;
          case 3: where=((w-1-i+x)*4)+((h-1-j+y)*tw*4); break;
        }
        ptr[where]=(int)(255.0*((((float)c.b/255.0)*res[4])
                  +(((float)ptr[where]/255.0)*(1.0-res[4]))));
        ptr[where+1]=(int)(255.0*((((float)c.g/255.0)*res[4])
                    +(((float)ptr[where+1]/255.0)*(1.0-res[4]))));
        ptr[where+2]=(int)(255.0*((((float)c.r/255.0)*res[4])
                    +(((float)ptr[where+2]/255.0)*(1.0-res[4]))));
        ptr[where+3]=(int)(255.0*(res[4]+(((float)ptr[where+3]/255.0)
                    *(1.0-res[4]))));
      }
    }
  }
  
  return 0;
}

int drawButton(SDL_Renderer* r, int x, int y, int w, int h, SDL_Color color, int rr) {
  SDL_Rect rect;
  rect.x=0; rect.y=0; rect.w=w; rect.h=h;
  SDL_Texture* t;
  t=SDL_CreateTexture(r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,w,h);
  void* p=NULL;
  int pitch;
  if (!SDL_LockTexture(t,NULL,&p,&pitch)) {
    unsigned char* pp;
    pp=new unsigned char[w*h*4];//(unsigned char*)p;
    
    unsigned char* ppp;
    ppp=(unsigned char*)p;
    
    memset(pp,0,w*h*4);
    
    roundRect(pp,w,{128,128,128,4},0,0,w,h,rr);
    roundRect(pp,w,{255,255,255,255},20,20,w-40,h-40,rr);
    
    // copy result
    for (int i=0; i<w*h; i++) {
      ppp[i*4]=pp[i*4];//color.b*pp[i*4]/255;
      ppp[i*4+1]=pp[i*4+1];//color.g*pp[i*4+1]/255;
      ppp[i*4+2]=pp[i*4+2];//color.r*pp[i*4+2]/255;
      ppp[i*4+3]=pp[i*4+3];//color.a;
    }
    delete[] pp;
    
    SDL_UnlockTexture(t);
  } else {
    fprintf(stderr,"can't.\n");
  }
  rect.x=x; rect.y=y;
  SDL_RenderCopy(r,t,NULL,&rect);
  SDL_DestroyTexture(t);
}
