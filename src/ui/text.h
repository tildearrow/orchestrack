// text field class
#include "../includes.h"
#include "button.h"
#include "../font/font.h"

class OTrackText {
  string* val;
  size_t maxlen, curpos, selpos;
  SDL_Rect pos;
  SDL_Rect spos;
  SDL_Renderer* rend;
  SDL_Texture* tex;
  font* f;
  SDL_Color fcolor;
  int blink;
  public:
    void draw();
    void setMaxLength(size_t max);
    void setOut(string* out);
    void setPos(int x, int y);
    void mouseMove(int x, int y);
    void mouseDown(int x, int y, int button);
    void mouseUp(int x, int y, int button);
    // not the same
    void keyEvent(int keycode, int vkeycode, int repeat, int stat);
    void keyInputEvent(char* code);
    OTrackText(SDL_Renderer* renderer, font* fo, int w, int h, unsigned char r, unsigned char g, unsigned char b);
};
