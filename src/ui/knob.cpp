#include "knob.h"

int circle(unsigned char* ptr, int tw, SDL_Color c, SDL_Color c1, int x, int y, int r) {
  float res[257]; // 16x sampling, period.
  unsigned char* bitmap;
  int where;
  // prepare bitmap
  bitmap=new unsigned char[r*r*16];
  memset(bitmap,0,r*r*16);
  
  // corners
  for (int i=0; i<r*2; i++) {
    for (int j=0; j<r*2; j++) {
      res[0]=0;
      for (int k=0; k<16; k++) {
        for (int l=0; l<16; l++) {
          if (pow(pow((float)r-(float)i-((float)k/16.0),2.0)+pow((float)r-(float)j-((float)l/16.0),2.0),0.5)<(r)) {
            res[0]++;
          }
        }
      }
      
      for (int k=0; k<4; k++) {
        switch (k) {
          case 0: where=((i)*4)+((j)*(r*2)*4); break;
          case 1: where=(((r*2)-1-i)*4)+((j)*(r*2)*4); break;
          case 2: where=((i)*4)+(((r*2)-1-j)*(r*2)*4); break;
          case 3: where=(((r*2)-1-i)*4)+(((r*2)-1-j)*(r*2)*4); break;
        }
        bitmap[where]=255;
        bitmap[where+1]=255;
        bitmap[where+2]=255;
        bitmap[where+3]=(unsigned char)(fmin(255,res[0]));
      }
    }
  }
  
  // apply color/gradient
  for (int i=0; i<r*2; i++) {
    for (int j=0; j<r*2; j++) {
      where=(i*4)+(j*(r*2)*4);
      bitmap[where]=(unsigned char)((float)c.b+(((float)c1.b-(float)c.b)*(float)j/(float)(r*2)));
      bitmap[where+1]=(unsigned char)((float)c.g+(((float)c1.g-(float)c.g)*(float)j/(float)(r*2)));
      bitmap[where+2]=(unsigned char)((float)c.r+(((float)c1.r-(float)c.r)*(float)j/(float)(r*2)));
      bitmap[where+3]=(unsigned char)((float)bitmap[where+3]*(((float)c.a+(((float)c1.a-(float)c.a)*(float)j/(float)(r*2)))/255.0));
    }
  }
  
  for (int i=x-r; i<x+r; i++) {
    for (int j=y-r; j<y+r; j++) {
      int src, dest;
      src=((i-x+r)*4)+((j-y+r)*(r*2)*4);
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

void OTrackKnob::draw(int x, int y) {
  SDL_Rect tr;
  SDL_Point tp;
  tr.x=x; tr.y=y; tr.w=w; tr.h=h;
  tp.x=w/2; tp.y=h/2;
  printf("value: %f\n",*val);
  SDL_RenderCopyEx(rend,tex,NULL,&tr,225+((*val)*270),&tp,SDL_FLIP_NONE);
}

void OTrackKnob::setOut(float* out) {
  val=out;
}

void OTrackKnob::setRange(float min, float max) {
  rmin=min;
  rmax=max;
}

OTrackKnob::OTrackKnob(SDL_Renderer* renderer, int rad, unsigned char r, unsigned char g, unsigned char b) {
  unsigned char* ba;
  rend=renderer;
  tex=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STATIC,rad*2,rad*2);
  ba=new unsigned char[r*r*16];
  memset(ba,0,r*r*16);
  circle(ba,r*2,{64,64,64,255},{96,96,96,255},rad,rad,rad-1);
  circle(ba,r*2,{192,192,192,255},{128,128,128,255},rad,rad,rad-2);
  circle(ba,r*2,{128,128,128,255},{96,96,96,255},rad,rad,rad-3);
  circle(ba,r*2,{128,128,128,255},{255,255,255,255},rad,rad,rad-4);
  circle(ba,r*2,{128,128,128,255},{128,128,128,255},rad,rad,rad-5);
  circle(ba,r*2,{200,200,200,192},{150,150,150,0},rad,rad,rad-7);
  circle(ba,r*2,{0,255,0,255},{0,255,0,255},rad,6,3);
  //ba[(r*16+r)*4+1]=0;
  SDL_UpdateTexture(tex,NULL,ba,r*8);
  delete[] ba;
  w=rad*2; h=rad*2;
}
