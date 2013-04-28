#include <cstdlib>
#include <ctime>
#include <csignal>
#include <iostream>

#include "track.h"
#include "car.h"
#include "car_factory/simple_car_factory.h"
#include "car_factory/csv_car_factory.h"
#include "config.h"
#include "statistics.h"

#ifdef GUI
#include <allegro.h>

BITMAP *buffer;
#endif

void initAllegro(int);
void deinitAllegro();
void sigHandler(int s);
void setupSignalHandler();

Track *track;
CarFactory *car_factory;
Statistics *statistics;
Config *config;

int main(int argc, char **argv) {

  track = NULL;

  setupSignalHandler();

  srand(time(NULL));

  config = new Config();
  config->loadFromFile("data/nash.config");

  statistics = new Statistics();

  bool running = true;

  //CarFactory *car_factory = new SimpleCarFactory(config);
  car_factory = new CsvCarFactory("data/samples.csv", config, statistics);

  const int track_length = config->getNumberOfTrackCells();
  const int track_max_speed = config->getTrackMaxSpeed();

  #ifdef GUI
  initAllegro(track_length);
  #endif

  track = new Track(car_factory, track_length, track_max_speed);

  #ifdef GUI
  while (!key[KEY_ESC]) {

    if (running) {
      clear_to_color(buffer, makecol(0, 0, 0));
      track->step();
      blit(buffer, screen, 0, 0, 0, 0, track_length, 10);
    }

    if (key[KEY_SPACE]) {
      running = !running;
    }

    rest(20);
  }
  #else
  //for (int i=0; i< 1000; i++) {
  while (track->isLive()) {
    track->step();
  }
  #endif

  statistics->print();

  delete car_factory;
  delete track;
  delete config;
  delete statistics;

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
  res = set_gfx_mode(GFX_AUTODETECT_WINDOWED, width, 10, 0, 0);
  if (res != 0) {
    allegro_message(allegro_error);
    exit(-1);
  }

  install_timer();
  install_keyboard();
  set_keyboard_rate(0, 0);
  buffer = create_bitmap(width, 10);
}

void deinitAllegro() {
  destroy_bitmap(buffer);
  clear_keybuf();
}
#endif

void sigHandler(int s) {
  if (car_factory) delete car_factory;
  if (track) delete track;
  if (config) delete config;
  if (statistics) delete statistics;
  exit(0);
}

void setupSignalHandler() {
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = sigHandler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);
}

