/* fextra.c - extra get/put functions
 * check fextra.h for more info
 */
#include "fextra.h"

/** READ FUNCTIONS **/

/* reading */
short fgetsh(FILE* f) {
  return (fgetc(f))|(fgetc(f)<<8);
}

int fgeti(FILE* f) {
  return (fgetc(f))|(fgetc(f)<<8)|
         (fgetc(f)<<16)|(fgetc(f)<<24);
}

long long fgetll(FILE* f) {
  return (fgetc(f))|(fgetc(f)<<8)|
         (fgetc(f)<<16)|(fgetc(f)<<24)|
         ((long long)fgetc(f)<<32)|((long long)fgetc(f)<<40)|
         ((long long)fgetc(f)<<48)|((long long)fgetc(f)<<56);
}

/* reading unsigned */
unsigned short fgetush(FILE* f) {
  return (fgetc(f))|(fgetc(f)<<8);
}

unsigned int fgetui(FILE* f) {
  return (fgetc(f))|(fgetc(f)<<8)|
         (fgetc(f)<<16)|(fgetc(f)<<24);
}

unsigned long long fgetull(FILE* f) {
  return (fgetc(f))|(fgetc(f)<<8)|
         (fgetc(f)<<16)|(fgetc(f)<<24)|
         ((long long)fgetc(f)<<32)|((long long)fgetc(f)<<40)|
         ((long long)fgetc(f)<<48)|((long long)fgetc(f)<<56);
}


/* big endian reading */
short fgetbsh(FILE* f) {
  return (fgetc(f)<<8)|(fgetc(f));
}

int fgetbi(FILE* f) {
  return (fgetc(f)<<24)|(fgetc(f)<<16)|
         (fgetc(f)<<8)|(fgetc(f));
}

long long fgetbll(FILE* f) {
  return ((long long)fgetc(f)<<56)|((long long)fgetc(f)<<48)|
         ((long long)fgetc(f)<<40)|((long long)fgetc(f)<<32)|
         (fgetc(f)<<24)|(fgetc(f)<<16)|
         (fgetc(f)<<8)|(fgetc(f));
}


/* big endian reading unsigned */
unsigned short fgetbush(FILE* f) {
  return (fgetc(f)<<8)|(fgetc(f));
}

unsigned int fgetbui(FILE* f) {
  return (fgetc(f)<<24)|(fgetc(f)<<16)|
         (fgetc(f)<<8)|(fgetc(f));
}

unsigned long long fgetbull(FILE* f) {
  return ((long long)fgetc(f)<<56)|((long long)fgetc(f)<<48)|
         ((long long)fgetc(f)<<40)|((long long)fgetc(f)<<32)|
         (fgetc(f)<<24)|(fgetc(f)<<16)|
         (fgetc(f)<<8)|(fgetc(f));
}


/* reading float/double */
float fgetf(FILE* f) {
  int v;
  float* r;
  v=((fgetc(f))|(fgetc(f)<<8)|
         (fgetc(f)<<16)|(fgetc(f)<<24));
  r=(float*)&v;
  return *r;
}

double fgetd(FILE* f) {
  long long v;
  double* r;
  v=((fgetc(f))|(fgetc(f)<<8)|
         (fgetc(f)<<16)|(fgetc(f)<<24)|
         ((long long)fgetc(f)<<32)|((long long)fgetc(f)<<40)|
         ((long long)fgetc(f)<<48)|((long long)fgetc(f)<<56));
  r=(double*)&v;
  return *r;
}


/* big endian reading float/double */
float fgetbf(FILE* f) {
  int v;
  float* r;
  v=((fgetc(f)<<24)|(fgetc(f)<<16)|
         (fgetc(f)<<8)|(fgetc(f)));
  r=(float*)&v;
  return *r;
}

double fgetbd(FILE* f) {
  long long v;
  double* r;
  v=(((long long)fgetc(f)<<56)|((long long)fgetc(f)<<48)|
         ((long long)fgetc(f)<<40)|((long long)fgetc(f)<<32)|
         (fgetc(f)<<24)|(fgetc(f)<<16)|
         (fgetc(f)<<8)|(fgetc(f)));
  r=(double*)&v;
  return *r;
}


/** WRITE FUNCTIONS **/

/* writing */
int fputsh(short c, FILE* f) {
  if ((fputc(c,f)==EOF)||(fputc(c>>8,f)==EOF)) {
    return EOF;
  }
  return 0;
}

int fputi(int c, FILE* f) {
  if ((fputc(c,f)==EOF)||(fputc(c>>8,f)==EOF)||
      (fputc(c>>16,f)==EOF)||(fputc(c>>24,f)==EOF)) {
    return EOF;
  }
  return 0;
}

int fputll(long long c, FILE* f) {
  if ((fputc((unsigned char)c,f)==EOF)||(fputc((unsigned short)c>>8,f)==EOF)||
      (fputc((unsigned int)c>>16,f)==EOF)||(fputc((unsigned int)c>>24,f)==EOF)||
      (fputc(c>>32,f)==EOF)||(fputc(c>>40,f)==EOF)||
      (fputc(c>>48,f)==EOF)||(fputc(c>>56,f)==EOF)) {
    return EOF;
  }
  return 0;
}


/* writing unsigned */
int fputush(unsigned short c, FILE* f) {
  if ((fputc(c,f)==EOF)||(fputc(c>>8,f)==EOF)) {
    return EOF;
  }
  return 0;
}

int fputui(unsigned int c, FILE* f) {
  if ((fputc(c,f)==EOF)||(fputc(c>>8,f)==EOF)||
      (fputc(c>>16,f)==EOF)||(fputc(c>>24,f)==EOF)) {
    return EOF;
  }
  return 0;
}

int fputull(unsigned long long c, FILE* f) {
  if ((fputc((unsigned char)c,f)==EOF)||(fputc((unsigned short)c>>8,f)==EOF)||
      (fputc((unsigned int)c>>16,f)==EOF)||(fputc((unsigned int)c>>24,f)==EOF)||
      (fputc(c>>32,f)==EOF)||(fputc(c>>40,f)==EOF)||
      (fputc(c>>48,f)==EOF)||(fputc(c>>56,f)==EOF)) {
    return EOF;
  }
  return 0;
}


/* big endian writing */
int fputbsh(short c, FILE* f) {
  if ((fputc(c>>8,f)==EOF)||(fputc(c,f)==EOF)) {
    return EOF;
  }
  return 0;
}

int fputbi(int c, FILE* f) {
  if ((fputc(c>>24,f)==EOF)||(fputc(c>>16,f)==EOF)||
      (fputc(c>>8,f)==EOF)||(fputc(c,f)==EOF)) {
    return EOF;
  }
  return 0;
}

int fputbll(long long c, FILE* f) {
  if ((fputc(c>>56,f)==EOF)||(fputc(c>>48,f)==EOF)||
      (fputc(c>>40,f)==EOF)||(fputc(c>>32,f)==EOF)||
      (fputc((unsigned int)c>>24,f)==EOF)||(fputc((unsigned int)c>>16,f)==EOF)||
      (fputc((unsigned short)c>>8,f)==EOF)||(fputc((unsigned char)c,f)==EOF)) {
    return EOF;
  }
  return 0;
}


/* big endian writing unsigned */
int fputbush(unsigned short c, FILE* f) {
  if ((fputc(c>>8,f)==EOF)||(fputc(c,f)==EOF)) {
    return EOF;
  }
  return 0;
}

int fputbui(unsigned int c, FILE* f) {
  if ((fputc(c>>24,f)==EOF)||(fputc(c>>16,f)==EOF)||
      (fputc(c>>8,f)==EOF)||(fputc(c,f)==EOF)) {
    return EOF;
  }
  return 0;
}

int fputbull(unsigned long long c,FILE* f) {
  if ((fputc(c>>56,f)==EOF)||(fputc(c>>48,f)==EOF)||
      (fputc(c>>40,f)==EOF)||(fputc(c>>32,f)==EOF)||
      (fputc((unsigned int)c>>24,f)==EOF)||(fputc((unsigned int)c>>16,f)==EOF)||
      (fputc((unsigned short)c>>8,f)==EOF)||(fputc((unsigned char)c,f)==EOF)) {
    return EOF;
  }
  return 0;
}


/* writing float/double */
int fputf(float c, FILE* f) {
  return EOF;
}

int fputd(double c, FILE* f) {
  return EOF;
}


/* big endian writing float/double */
int fputbf(float c, FILE* f) {
  return EOF;
}

int fputbd(double c, FILE* f) {
  return EOF;
}
