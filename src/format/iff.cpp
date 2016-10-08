/// xxd -g 1 loop.wav:

/*****************************************************************************\
| 00000000: 52 49 46 46 e8 00 00 00 57 41 56 45 66 6d 74 20  RIFF....WAVEfmt  |
| 00000010: 10 00 00 00 01 00 01 00 ab 20 00 00 ab 20 00 00  ......... ... .. |
| 00000020: 01 00 08 00 64 61 74 61 40 00 00 00 80 80 ec e3  ....data@....... |
| 00000030: df d8 d5 d2 cf cc c9 c6 c1 bd b8 ad a8 a2 9a 95  ................ |
| 00000040: 91 8d 7f 7c 75 73 68 64 61 5a 55 4d 80 80 c8 c9  ...|ushdaZUM.... |
| 00000050: c9 c9 c9 c9 c9 c5 bf b4 a3 93 82 7b 7b 7b 81 85  ...........{{{.. |
| 00000060: 8e 93 9e a6 ab b1 b7 bc c1 c2 c5 c6 73 6d 70 6c  ............smpl |
| 00000070: 3c 00 00 00 00 00 00 00 00 00 00 00 16 d3 01 00  <............... |
| 00000080: 3c 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  <............... |
| 00000090: 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................ |
| 000000a0: 20 00 00 00 31 00 00 00 00 00 00 00 00 00 00 00   ...1........... |
| 000000b0: 78 74 72 61 10 00 00 00 00 00 00 00 80 00 00 01  xtra............ |
| 000000c0: 40 00 00 00 00 00 00 00 4c 49 53 54 20 00 00 00  @.......LIST ... |
| 000000d0: 49 4e 46 4f 49 53 46 54 13 00 00 00 4f 70 65 6e  INFOISFT....Open |
| 000000e0: 4d 50 54 20 31 2e 32 36 2e 30 35 2e 30 30 00 00  MPT 1.26.05.00.. |
\*****************************************************************************/

#include "iff.h"

iff* readIFF(FILE* f) {
  iff* r;
  int curc;
  curc=0;
  r=new iff;
  r->isRIFF=false;
  fseek(f,0,SEEK_SET);
  r->h.id[0]=fgetc(f);
  r->h.id[1]=fgetc(f);
  r->h.id[2]=fgetc(f);
  r->h.id[3]=fgetc(f);
  r->h.id[4]=0;
  if (strcmp(r->h.id,"RIFF")!=0 && strcmp(r->h.id,"FORM")!=0) {
    delete r;
    return NULL;
  }
  if (strcmp(r->h.id,"RIFF")==0) {
    printf("is RIFF\n");
    r->isRIFF=true;
    r->h.size=fgeti(f);
  } else if (strcmp(r->h.id,"FORM")==0) {
    printf("is IFF\n");
    r->h.size=fgetbi(f);
  } else {
    printf("is some IFF derivative. failing\n");
    delete r;
    return NULL;
  }
  //printf("has size %d\n",r->h.size);
  r->h.format[0]=fgetc(f);
  r->h.format[1]=fgetc(f);
  r->h.format[2]=fgetc(f);
  r->h.format[3]=fgetc(f);
  r->h.format[4]=0;
  //printf("type: %s\n",r->h.format);
  //printf("begin read subchunks\n");
  while (!feof(f)) {
    r->s.resize(r->s.size()+1);
    r->s[curc].id[0]=fgetc(f);
    r->s[curc].id[1]=fgetc(f);
    r->s[curc].id[2]=fgetc(f);
    r->s[curc].id[3]=fgetc(f);
    r->s[curc].id[4]=0;
    if (r->isRIFF) {
      r->s[curc].size=fgeti(f);
    } else {
      r->s[curc].size=fgetbi(f);
    }
    if (feof(f)) {
      r->s.resize(r->s.size()-1);
      break;
    }
    printf("-CHUNK %s (%ld bytes)-\n",r->s[curc].id,r->s[curc].size);
    r->s[curc].data=new unsigned char[r->s[curc].size];
    //printf("DATA:");
    for (int i=0; i<r->s[curc].size; i++) {
      r->s[curc].data[i]=fgetc(f);
      //printf(" %.2x",r->s[curc].data[i]);
    }
    //printf("\n");
    curc++;
  }
  return r;
}

void freeIFF(iff* r) {
  for (size_t i=0; i<r->s.size(); i++) {
    delete[] r->s[i].data;
  }
  delete r;
}
