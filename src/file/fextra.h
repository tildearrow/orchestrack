/* fextra.h - extra get/put functions
 * usage:
 * - same as fgetc/fputc
 * warnings:
 * - does not perform EOF checks
 * - long long functions may not works on
 *   non-64-bit archs
 * - not tested on big endian
 */
#include <stdio.h>

/** READ FUNCTIONS **/

/* reading */
short fgetsh(FILE* f);
int fgeti(FILE* f);
long long fgetll(FILE* f);

/* reading unsigned */
unsigned short fgetush(FILE* f);
unsigned int fgetui(FILE* f);
unsigned long long fgetull(FILE* f);

/* big endian reading */
short fgetbsh(FILE* f);
int fgetbi(FILE* f);
long long fgetbll(FILE* f);

/* big endian reading unsigned */
unsigned short fgetbush(FILE* f);
unsigned int fgetbui(FILE* f);
unsigned long long fgetbull(FILE* f);

/* reading float/double */
float fgetf(FILE* f);
double fgetd(FILE* f);

/* big endian reading float/double */
float fgetbf(FILE* f);
double fgetbd(FILE* f);

/** WRITE FUNCTIONS **/

/* writing */
int fputsh(short c, FILE* f);
int fputi(int c, FILE* f);
int fputll(long long c, FILE* f);

/* writing unsigned */
int fputush(unsigned short c, FILE* f);
int fputui(unsigned int c, FILE* f);
int fputull(unsigned long long c, FILE* f);

/* big endian writing */
int fputbsh(short c, FILE* f);
int fputbi(int c, FILE* f);
int fputbll(long long c, FILE* f);

/* big endian writing unsigned */
int fputbush(unsigned short c, FILE* f);
int fputbui(unsigned int c, FILE* f);
int fputbull(unsigned long long c,FILE* f);

/* writing float/double */
int fputf(float c, FILE* f);
int fputd(double c, FILE* f);

/* big endian writing float/double */
int fputbf(float c, FILE* f);
int fputbd(double c, FILE* f);
