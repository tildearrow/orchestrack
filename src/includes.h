#ifndef INCLUDES_H
#define INCLUDES_H
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
//#include <json.hpp>
#include <vector>
#include <string>
#include <queue>
#define pi 3.141592653589
#define PointInRect(x, y, x1, y1, x2, y2)\
  (x>x1 && x<x2 && y>y1 && y<y2)?(1):(0)
typedef std::string string;
#endif
