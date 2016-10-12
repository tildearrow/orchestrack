#include "sampler.h"

float Sampler::intNone(float* b, int n, float d) {
  return b[n];
}

float Sampler::intLinear(float* b, int n, float d) {
  return b[n]+((b[n+1]-b[n])*d);
}

// slightly modified version of http://www.musicdsp.org/showone.php?id=49
float Sampler::intCubic(float* b, int n, float d) {
  float a, bb, c;
  a=(3*(b[n]-b[n+1])-b[n-1]+b[n+2])/2;
  bb=2*b[n+1]+b[n-1]-(5*b[n]+b[n+2])/2;
  c=(b[n+1]-b[n-1])/2;
  return (((a*d)+bb)*d+c)*d+b[n];
}

float Sampler::intSinc(float* b, int n, float d) {
  int pl;
  float* where;
  pl=((int)(d*65536))&0xffff;
  where=table+(((pl+fHalve)>>fShift)&fMask);
  return b[n-3]*where[0]+
         b[n-3]*where[1]+
         b[n-1]*where[2]+
         b[ n ]*where[3]+
         b[n+1]*where[4]+
         b[n+2]*where[5]+
         b[n+3]*where[6]+
         b[n+4]*where[7];
}
