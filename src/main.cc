#include <cstdlib>
#include <ctime>
#include <csignal>
#include <iostream>
#include <unistd.h>
#include <cstring>

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
  int screen_height;
  #define LINE_HEIGHT 2
#endif

// carfactory
// 1 - simple
// 2 - csv car factory
#ifndef CARFACTORY
  #define CARFACTORY 1
#endif

void initAllegro(int);
void deinitAllegro();
void sigHandler(int s);
void setupSignalHandler();

Track *track;
CarFactory *car_factory;
Statistics *statistics;
Config *config;

using std::cout;

// kontrola, zda klavesa byla naposled stisknuta alespon pred 1s
bool should_execute(int letter) {

   static int key_last_press[256] = {0};
   if(!key_last_press[0])
     memset(key_last_press, 2, sizeof(int) * 256);

   time_t last_press = time(NULL);
   if((last_press - key_last_press[letter]) >= 1) {
      key_last_press[letter] = last_press;
      return true;
   }
   return false;
}

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

  #if CARFACTORY==1
    CarFactory *car_factory = new SimpleCarFactory(config, statistics);
  #elif CARFACTORY==2
    if (argc >=3) {
      car_factory = new CsvCarFactory(argv[2], config, statistics);
      std::cout << "Samples loaded from '" << argv[2] << "'.\n";
    } else {
      car_factory = new CsvCarFactory("data/samples.csv", config, statistics);
    }
  #endif

  const int track_length = config->getNumberOfTrackCells();

  #ifdef GUI
  initAllegro(track_length);
  #endif

  track = new Track(car_factory, track_length);

  #ifdef GUI

    BITMAP * buffer2 = create_bitmap(screen_width, screen_height);

    int colors[] = {
      makecol(255,0,0),
      makecol(100,0,0),
      makecol(100,100,0),
      makecol(0,200,0),
      makecol(0,255,0),
      makecol(0, 0, 255),
      makecol(255, 0, 0),
      makecol(255, 0, 0),
      makecol(255, 0, 0)
    };

    bool running = true, stop = false;
    clear_to_color(screen, makecol(255, 255, 255));

    while (!key[KEY_ESC] && track->isLive()) {

      if (running) {
        track->step();

        unsigned long long current_time = track->getCurrentTime();
        int offset = (current_time*LINE_HEIGHT)%(screen_height-LINE_HEIGHT);

        if (offset == 0) {
          if (stop) {
            stop = false;
            running = false;
            continue;
          }
          clear_to_color(screen, makecol(255, 255, 255));
        }
        clear_to_color(buffer, makecol(255, 255, 255));
        clear_to_color(buffer2, makecol(255, 255, 255));
        // std::cout << offset << std::endl;

        /**
         * Vykresleni cesty
         */
        Cell * cell = track->getFirstCell();
        while (cell != NULL) {
          if (cell->isOccupied()) {

            line(buffer, cell->getPosition(), 0, cell->getPosition(), LINE_HEIGHT,
              makecol(0,0,0));
              // makecol((255/(cell->getCar()->getCurrentSpeed()+1)), (cell->getCar()->getCurrentSpeed()-1)*30, 0));
              // colors[cell->getCar()->getCurrentSpeed()-1]);

            std::cout << cell->getCar()->getCurrentSpeed();
          } else {
            std::cout << '.';
          }
          cell = cell->getCellFront();
        }
        std::cout << std::endl;
        stretch_blit(buffer, buffer2, 0, 0, buffer->w, buffer->h, 0, 0, buffer2->w, buffer2->h);
        blit(buffer2, screen, 0, 0, 0, offset, screen_width, LINE_HEIGHT);
      } else {
        rest(40);
      }

      if (key[KEY_SPACE]) {
        if (should_execute(KEY_SPACE)) {
          if (running) {
            stop = true;
            cout << "Stop\n";
          } else {
            running = true;
            clear_to_color(screen, makecol(255, 255, 255));
            cout << "Run\n";
          }
        }
      }
    }
    destroy_bitmap(buffer2);

  #elif ASCII

    while (track->isLive()) {

      track->step();
      Cell * cell = track->getFirstCell();

      while (cell != NULL) {

        if (cell->isOccupied()) {
          std::cout << cell->getCar()->getCurrentSpeed();
        } else {
          std::cout << '.';
        }
        cell = cell->getCellFront();
      }
      std::cout << std::endl;
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

  // ziskani rozliseni obrazovky, pokud nelze, nastavi se default sirka
  if (get_desktop_resolution(&screen_width, &screen_height) != 0) {
    screen_width = 1000;
    screen_height = 200 * LINE_HEIGHT;
  }
  // pokud je pripojeno vice monitoru, nechceme zobrazovat na vsech, omezeni na full hd
  if (screen_width > 1980) screen_width = 1980;
  if (depth == 0) depth = 32;
  screen_width -= 100;
  screen_height -= 100;

  set_color_depth(depth);
  res = set_gfx_mode(GFX_AUTODETECT_WINDOWED, screen_width, screen_height, 2*screen_width, 0);
  if (res != 0) {
    exit(-1);
  }

  install_timer();
  install_keyboard();
  set_keyboard_rate(0, 0);
  buffer = create_bitmap(width, LINE_HEIGHT);
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

