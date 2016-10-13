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

void OTrackKnob::mouseMove(int x, int y) {
  if (drag) {
    *val=fmax(rmin,fmin(rmax,dragVal+((float)(dragStart-y)/300.0)));
  } else {
    if (PointInRect(x,y,xx,yy,xx+w,yy+h)) {
      hover=true;
    } else {
      hover=false;
    }
  }
}

void OTrackKnob::mouseDown(int x, int y, int button) {
  if (PointInRect(x,y,xx,yy,xx+w,yy+h)) {
    hover=true;
  } else {
    hover=false;
  }
  if (hover && button==0) {
    reset=false;
    drag=true;
    dragStart=y;
    dragVal=*val;
    SDL_CaptureMouse(SDL_TRUE);
  } else if ((button==1 || button==2) && !drag) {
    reset=true;
  }
}

void OTrackKnob::mouseUp(int x, int y, int button) {
  if (drag && button==0) {
    drag=false;
    SDL_CaptureMouse(SDL_FALSE);
    if (!PointInRect(x,y,xx,yy,xx+w,yy+h)) {
      hover=false;
    }
  }
}

void OTrackKnob::draw() {
  SDL_Rect tr;
  SDL_Point tp;
  tr.x=xx; tr.y=yy; tr.w=w; tr.h=h;
  tp.x=w/2; tp.y=h/2;
  SDL_RenderCopy(rend,tex,NULL,&tr);
  
  SDL_SetTextureColorMod(light,127+hoverTime*16,127+hoverTime*16,127+hoverTime*16);

  for (int i=0; i<(*val)*32; i++) {
    tr.x=round((xx+(w/2)+cos((0.75*pi)+(float)i*1.5*pi/(32.0))*(w-18)/2)-6);
    tr.y=round((yy+(h/2)+sin((0.75*pi)+(float)i*1.5*pi/(32.0))*(h-18)/2)-6);
    
    tr.w=12; tr.h=12;
    if (i==(int)((*val)*32)) {
      SDL_SetTextureAlphaMod(light,(unsigned char)(fmod((*val)*32,1)*255));
    }
    SDL_RenderCopy(rend,light,NULL,&tr);
    SDL_SetTextureAlphaMod(light,255);
  }
  tr.x=xx+8; tr.y=yy+8; tr.w=w-16; tr.h=h-16;
  tp.x=(w-16)/2; tp.y=(h-16)/2;
  SDL_RenderCopyEx(rend,tex1,NULL,&tr,225+((*val)*270),&tp,SDL_FLIP_NONE);

  if (hover || drag) {
    hoverTime++;
    if (hoverTime>8) {
      hoverTime=8;
    }
  } else {
    hoverTime--;
    if (hoverTime<0) {
      hoverTime=0;
    }
  }
  
  if (reset) {
    *val-=((*val)-rval)*0.2;
    if (fabs(*val-rval)<0.005) {
      *val=rval;
      reset=false;
    }
  }
}

void OTrackKnob::setOut(float* out) {
  val=out;
}

void OTrackKnob::setRange(float min, float max, float resetval) {
  rmin=min;
  rmax=max;
  rval=resetval;
}

void OTrackKnob::setPos(int x, int y) {
  xx=x; yy=y;
}

OTrackKnob::OTrackKnob(SDL_Renderer* renderer, int rad, unsigned char r, unsigned char g, unsigned char b) {
  unsigned char* ba;
  unsigned char* bb;
  unsigned char* bc;
  SDL_Color tc, tc1;
  rend=renderer;
  tex=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STATIC,rad*2,rad*2);
  tex1=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STATIC,(rad-8)*2,(rad-8)*2);
  light=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STATIC,12,12);
  SDL_SetTextureBlendMode(tex,SDL_BLENDMODE_BLEND);
  SDL_SetTextureBlendMode(tex1,SDL_BLENDMODE_BLEND);
  SDL_SetTextureBlendMode(light,SDL_BLENDMODE_ADD);
  ba=new unsigned char[rad*rad*16];
  memset(ba,0,rad*rad*16);
  bc=new unsigned char[((rad-8)*2)*((rad-8)*2)*4];
  memset(bc,0,((rad-8)*2)*((rad-8)*2)*4);
  bb=new unsigned char[12*12*4];
  memset(bb,0,12*12*4);
  
  // knob
  
  tc.r=64; tc.g=64; tc.b=64; tc.a=255;
  tc1.r=96; tc1.g=96; tc1.b=96; tc1.a=255;
  circle(ba,rad*2,tc,tc1,rad,rad,rad-1);
  
  tc.r=192; tc.g=192; tc.b=192; tc.a=255;
  tc1.r=128; tc1.g=128; tc1.b=128; tc1.a=255;
  circle(ba,rad*2,tc,tc1,rad,rad,rad-3);
  
  tc.r=32; tc.g=32; tc.b=32; tc.a=255;
  tc1.r=16; tc1.g=16; tc1.b=16; tc1.a=255;
  circle(ba,rad*2,tc,tc1,rad,rad,rad-4);
  
  // knob center
  
  tc.r=224; tc.g=224; tc.b=224; tc.a=255;
  tc1.r=160; tc1.g=160; tc1.b=160; tc1.a=255;
  circle(bc,(rad-8)*2,tc,tc1,rad-8,rad-8,rad-9);
  
  tc.r=64; tc.g=64; tc.b=64; tc.a=255;
  tc1.r=128; tc1.g=128; tc1.b=128; tc1.a=255;
  circle(bc,(rad-8)*2,tc,tc1,rad-8,rad-8,rad-10);
  
  tc.r=255; tc.g=255; tc.b=255; tc.a=128;
  tc1.r=0; tc1.g=0; tc1.b=0; tc1.a=0;
  circle(bc,(rad-8)*2,tc,tc1,rad-8,rad-8,rad-12);
  
  // indicator
  
  tc.r=128; tc.g=128; tc.b=128; tc.a=255;
  circle(bc,(rad-8)*2,tc,tc,rad-8,7,2);
  circle(bc,(rad-8)*2,tc,tc,rad-8,8,2);
  circle(bc,(rad-8)*2,tc,tc,rad-8,9,2);
  circle(bc,(rad-8)*2,tc,tc,rad-8,10,2);
  circle(bc,(rad-8)*2,tc,tc,rad-8,11,2);
  circle(bc,(rad-8)*2,tc,tc,rad-8,12,2);
  circle(bc,(rad-8)*2,tc,tc,rad-8,13,2);
  circle(bc,(rad-8)*2,tc,tc,rad-8,14,2);
  
  tc.r=255; tc.g=255; tc.b=255; tc.a=255;
  circle(bc,(rad-8)*2,tc,tc,rad-8,6,1);
  circle(bc,(rad-8)*2,tc,tc,rad-8,7,1);
  circle(bc,(rad-8)*2,tc,tc,rad-8,8,1);
  circle(bc,(rad-8)*2,tc,tc,rad-8,9,1);
  circle(bc,(rad-8)*2,tc,tc,rad-8,10,1);
  circle(bc,(rad-8)*2,tc,tc,rad-8,11,1);
  circle(bc,(rad-8)*2,tc,tc,rad-8,12,1);
  circle(bc,(rad-8)*2,tc,tc,rad-8,13,1);
  circle(bc,(rad-8)*2,tc,tc,rad-8,14,1);
  circle(bc,(rad-8)*2,tc,tc,rad-8,15,1);
  
  // light
  
  tc.r=r; tc.g=g; tc.b=b; tc.a=96;
  circle(bb,12,tc,tc,6,6,5);
  tc.r=r; tc.g=g; tc.b=b; tc.a=128;
  circle(bb,12,tc,tc,6,6,4);
  tc.r=r; tc.g=g; tc.b=b; tc.a=144;
  circle(bb,12,tc,tc,6,6,3);
  tc.r=r; tc.g=g; tc.b=b; tc.a=160;
  circle(bb,12,tc,tc,6,6,2);
  tc.r=r; tc.g=g; tc.b=b; tc.a=255;
  circle(bb,12,tc,tc,6,6,1);
  
  SDL_UpdateTexture(tex,NULL,ba,rad*8);
  SDL_UpdateTexture(tex1,NULL,bc,(rad-8)*8);
  SDL_UpdateTexture(light,NULL,bb,12*4);
  delete[] ba;
  delete[] bb;
  delete[] bc;
  w=rad*2; h=rad*2;
}
