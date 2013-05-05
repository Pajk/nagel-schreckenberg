#include <cstdlib>
#include <ctime>
#include <csignal>
#include <iostream>
#include <unistd.h>

#include "track.h"
#include "car.h"
#include "car_factory/simple_car_factory.h"
#include "car_factory/csv_car_factory.h"
#include "config.h"
#include "statistics.h"
#include "cell.h"

#ifdef GUI
#include <allegro.h>

BITMAP *buffer;
int screen_width;
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

  srand(time(NULL)*getpid());

  config = new Config();

  if (argc >= 2) {
    config->loadFromFile(argv[1]);
    std::cout << "Settings loaded from '" << argv[1] << "'.\n";
  } else {
    config->loadFromFile("data/default.config");
  }
  config->print();

  statistics = new Statistics();

  bool running = true;

  //CarFactory *car_factory = new SimpleCarFactory(config);
  if (argc >=3) {
    car_factory = new CsvCarFactory(argv[2], config, statistics);
    std::cout << "Samples loaded from '" << argv[2] << "'.\n";
  } else {
    car_factory = new CsvCarFactory("data/samples.csv", config, statistics);
  }

  const int track_length = config->getNumberOfTrackCells();

  #ifdef GUI
  initAllegro(track_length);
  #endif

  track = new Track(car_factory, track_length);

  #ifdef GUI
  BITMAP *b;
  int red = makecol(255,0,0);
  int green = makecol(0,255,0);
  while (!key[KEY_ESC]) {

    if (running) {
      clear_to_color(buffer, makecol(0, 0, 0));
      track->step();

      /**
       * Vykresleni cesty
       */
      Cell * tmp = track->getFirstCell();
      while (tmp) {
        if (tmp->isOccupied()) {
          int car_class = tmp->getCar()->getCarClass();
          if (car_class < 2) {
            // rychlejsi
            line(buffer, tmp->getPosition(), 0, tmp->getPosition(), 10, green);
          } else {
            // pomalejsi auta
            line(buffer, tmp->getPosition(), 0, tmp->getPosition(), 10, red);
          }

        }
        tmp = tmp->getCellFront();
      }

      b = create_bitmap(screen_width, 10);
      stretch_blit(buffer, b, 0, 0, buffer->w, buffer->h, 0, 0, b->w, b->h);
      blit(b, screen, 0, 0, 0, 0, screen_width, 10);
      destroy_bitmap(b);
    }

    if (key[KEY_SPACE]) {
      running = !running;
    }

    rest(10);
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

  int depth, res, h;
  allegro_init();
  depth = desktop_color_depth();

  // ziskani rozliseni obrazovky, pokud nelze, nastavi se default sirka
  if (get_desktop_resolution(&screen_width, &h) != 0) screen_width = 1440;
  if (screen_width > 1300) screen_width = 1300;
  if (depth == 0) depth = 32;
  set_color_depth(depth);
  res = set_gfx_mode(GFX_AUTODETECT_WINDOWED, screen_width, 10, 0, 0);
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

