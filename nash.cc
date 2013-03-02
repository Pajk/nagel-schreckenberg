#include <allegro.h>

#include "track.h"
#include "car.h"

BITMAP *buffer;

void initAllegro(int);
void deinitAllegro();

int main(int argc, char **argv) {

    srand(time(NULL));  

    int track_length = 1430;
    
    initAllegro(track_length);

    Track *track = new Track(track_length);

    while (!key[KEY_ESC]) {
      clear_to_color(buffer, makecol(0, 0, 0));
      track->step();
      blit(buffer, screen, 0, 0, 0, 0, track_length, 50);
      rest(20);
    }
    
    delete track;

    deinitAllegro();

    return 0;
}
END_OF_MAIN()

void initAllegro(int width) {

  int depth, res;
  allegro_init();
  depth = desktop_color_depth();
  if (depth == 0) depth = 32;
  set_color_depth(depth);
  res = set_gfx_mode(GFX_AUTODETECT_WINDOWED, width, 50, 0, 0);
  if (res != 0) {
    allegro_message(allegro_error);
    exit(-1);
  }

  install_timer();
  install_keyboard();
  buffer = create_bitmap(width, 50);
}

void deinitAllegro() {
  destroy_bitmap(buffer);
  clear_keybuf();
}


