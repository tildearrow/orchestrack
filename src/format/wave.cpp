#include "wave.h"

rwave* readWAVE(riff* f) {
  rwave* r;
  r=new rwave;
  r->smpl.loops=0;
  r->smpl.l.resize(0);
  for (int i=0; i<f->s.size(); i++) {
    if (strcmp(f->s[i].id,"fmt ")==0) {
      printf("format chunk!\n");
      r->fmt.aformat=f->s[i].data[0]+(f->s[i].data[1]<<8);
      r->fmt.chan=f->s[i].data[2]+(f->s[i].data[3]<<8);
      r->fmt.srate=f->s[i].data[4]+(f->s[i].data[5]<<8)+
                  (f->s[i].data[6]<<16)+(f->s[i].data[7]<<24);
      r->fmt.byterate=f->s[i].data[8]+(f->s[i].data[9]<<8)+
                      (f->s[i].data[10]<<16)+(f->s[i].data[11]<<24);
      r->fmt.bal=f->s[i].data[12]+(f->s[i].data[13]<<8);
      r->fmt.bps=f->s[i].data[14]+(f->s[i].data[15]<<8);
      printf("- aformat: %d\n",r->fmt.aformat);
      printf("- channels: %d\n",r->fmt.chan);
      printf("- rate: %d\n",r->fmt.srate);
      printf("- brate: %d\n",r->fmt.byterate);
      printf("- bal: %d\n",r->fmt.bal);
      printf("- bps: %d\n",r->fmt.bps);
    } else if (strcmp(f->s[i].id,"data")==0) {
      // NOTE: please enable later.
      /***************
      // read data.
      r->data.size=f->s[i].size;
      unsigned char* data;
      data=new unsigned char[r->data.size];
      r->data.size=f->s[i].size/(1+((r->fmt.bps-1)/8));
      r->data.data=new float[r->data.size];
      printf("data chunk!\n");
      switch ((r->fmt.bps-1)/8) {
        case 0: // char
          char* buf8;
	  buf8=(char*)data;
	  printf("8 bit:");
          for (int j=0; j<r->data.size; j++) {
             buf8[j]=f->s[i].data[j]>>(8-(1+(r->fmt.bps-1%8)));
	     r->data.data[j]=(float)buf8[j]/128.0;
	     printf(" %.2x",buf8[j]);
	  }
	  printf("\n");
          break;
        case 1: // short
          short* buf16, *data16;
	  buf16=(short*)data;
	  data16=(short*)f->s[i].data;
	  printf("16 bit:");
          for (int j=0; j<r->data.size; j++) {
             buf16[j]=data16[j]>>(8-(1+(r->fmt.bps-1%8)));
	     r->data.data[j]=(float)buf16[j]/32768.0;
	     printf(" %.4x",buf16[j]);
	  }
	  printf("\n");
          break;
        case 3: // int
          int* buf32, *data32, *floathack;
	  buf32=(int*)data;
	  floathack=(int*)r->data.data;
	  data32=(int*)f->s[i].data;
	  printf("32 bit:");
          for (int j=0; j<r->data.size; j++) {
             buf32[j]=data32[j]>>(8-(1+(r->fmt.bps-1%8)));
	     if (r->fmt.aformat==3) {
	       floathack[j]=buf32[j];
	     } else {
	       r->data.data[j]=(float)buf16[j]/2147483648.0;
	     }
	     printf(" %.8x",buf32[j]);
	  }
	  printf("\n");
          break;
        case 7: // long long
          long long* buf64, *data64;
	  buf64=(long long*)data;
	  data64=(long long*)f->s[i].data;
	  printf("64 bit:");
          for (int j=0; j<r->data.size; j++) {
             buf64[j]=data64[j]>>(8-(1+(r->fmt.bps-1%8)));
	     printf(" %.16x",buf64[j]);
	  }
	  printf("\n");
          break;
        default:
	  printf("to be implemented later! %d\n",(r->fmt.bps-1)/8);
      }
      printf("converted to float:\n"); 
      for (int j=0; j<r->data.size; j++) {
        printf("%f ",r->data.data[j]);
      }
      printf("\n");
      ********/
    } else if (strcmp(f->s[i].id,"smpl")==0) {
      printf("THE chunk!\n");
      r->smpl.loops=f->s[i].data[28]+(f->s[i].data[29]<<8)+
                    (f->s[i].data[30]<<16)+(f->s[i].data[31]<<24);
                    printf("loops: %ld\n",r->smpl.loops);
      r->smpl.l.resize(r->smpl.loops);
      for (int j=0; j<r->smpl.loops; j++) {
        r->smpl.l[j].id=f->s[i].data[36]+(f->s[i].data[37]<<8)+
                        (f->s[i].data[38]<<16)+(f->s[i].data[39]<<24);
        r->smpl.l[j].type=f->s[i].data[40]+(f->s[i].data[41]<<8)+
                          (f->s[i].data[42]<<16)+(f->s[i].data[43]<<24);
        r->smpl.l[j].start=f->s[i].data[44]+(f->s[i].data[45]<<8)+
                           (f->s[i].data[46]<<16)+(f->s[i].data[47]<<24);
        r->smpl.l[j].end=f->s[i].data[48]+(f->s[i].data[49]<<8)+
                         (f->s[i].data[50]<<16)+(f->s[i].data[51]<<24);
        r->smpl.l[j].frac=f->s[i].data[52]+(f->s[i].data[53]<<8)+
                         (f->s[i].data[54]<<16)+(f->s[i].data[55]<<24);
        r->smpl.l[j].pc=f->s[i].data[56]+(f->s[i].data[57]<<8)+
                         (f->s[i].data[58]<<16)+(f->s[i].data[59]<<24);
        printf("loop %d:\n",j);
        printf("-ID %ld:\n",r->smpl.l[j].id);
        printf("-type %ld:\n",r->smpl.l[j].type);
        printf("-start %ld:\n",r->smpl.l[j].start);
        printf("-end %ld:\n",r->smpl.l[j].end);
        printf("-frac %ld:\n",r->smpl.l[j].frac);
        printf("-pc %ld:\n",r->smpl.l[j].pc);
      }
    } else {
      printf("other chunk!\n");
    }
  }
  return r;
}

void freeWAVE(rwave* r) {
  delete r;
}
