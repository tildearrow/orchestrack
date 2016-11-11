#ifndef INCLUDES_H
#define INCLUDES_H
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define DIR_SEP '\\'
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#define DIR_SEP '/'
#endif
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../lib/gfx/SDL2_gfxPrimitives.h"
//#include <json.hpp>
#include <vector>
#include <string>
#include <queue>
#define pi 3.141592653589
#define PointInRect(x, y, x1, y1, x2, y2)\
  (x>x1 && x<x2 && y>y1 && y<y2)?(1):(0)
#define panl(x) ((x>0)?(1-x):(1))
#define panr(x) ((x<0)?(1+x):(1))
typedef std::string string;
const char sChromaNote[12]={'C','C','D','D','E','F','F','G','G','A','A','B'};
const char sChromaSemitone[12]={'-','#','-','#','-','-','#','-','#','-','#','-'};
const char fSuffix[9]={'\0','K','M','G','T','P','E','Z','Y'};
#endif
