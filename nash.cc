#include <cstdlib>
#include <ctime>
#include <csignal>
#include <iostream>

#include "track.h"
#include "car.h"

#ifdef GUI
#include <allegro.h>

BITMAP *buffer;
#endif

void initAllegro(int);
void deinitAllegro();
void sigHandler(int s);

Track *track;

int main(int argc, char **argv) {
    
  track = NULL;
  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = sigHandler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

  srand(time(NULL));  

  int track_length = 1430;
  
  #ifdef GUI
  initAllegro(track_length);
  #endif

  track = new Track(track_length);
  
  #ifdef GUI
  while (!key[KEY_ESC]) {
    clear_to_color(buffer, makecol(0, 0, 0));
    track->step();
    blit(buffer, screen, 0, 0, 0, 0, track_length, 50);
    rest(20);
  }
  #else
  //for (int i=0; i< 1000; i++) {
  while(1) {
    track->step();
  }
  #endif
  
  delete track;
  track = NULL;

  #ifdef GUI
  deinitAllegro();
  #endif

  return 0;
}
#ifdef GUI
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
#endif

void sigHandler(int s) {
  std::cout << "koncim" << std::endl;
  if (track) {
    delete track;
  }
  exit(0);
}
