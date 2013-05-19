/**
 * Autor: Pavel Pokorny
 * Datum: 18.5.2013
 */

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
#include "car_factory/normal_car_factory.h"
#include "config.h"
#include "statistics.h"
#include "cell.h"
#include "world.h"

#ifdef GUI
#include <allegro.h>
BITMAP *buffer;
int screen_width;
int screen_height;
#define LINE_HEIGHT 2
#endif

#define DEAULT_SAMPLES_FILE "data/samples.csv"

void initAllegro(int);
void deinitAllegro();
void freeMemory(int s = -1);
void setupSignalHandler();
bool should_execute(int letter);
CarFactory * getCarFactory(Config * config, int argc, char ** argv);
Config * getConfig(int argc, char ** argv);
void guiLoop(World * world);
void asciiLoop(World * world);
void loop(World * world);

CarFactory * car_factory;
Config * config;

using std::cout;
using std::endl;

int main(int argc, char **argv) {

  setupSignalHandler();

  srand(time(NULL)*getpid());

  config = getConfig(argc, argv);
  config->print();
  car_factory = getCarFactory(config, argc, argv);
  Statistics statistics(config->useTableFormat());
  World world(&statistics, config);

  Track track(config, car_factory);
  world.addTrack(&track);

  #if defined(GUI)
    guiLoop(&world);
  #elif defined(ASCII)
    asciiLoop(&world);
  #else
    loop(&world);
  #endif

  statistics.calculatePrintAndReset(world.getCurrentTime());
  statistics.summaryCalculateAndPrint();
  freeMemory();

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

/**
 * Vycisteni alokovane pameti
 */
void freeMemory(int s) {
  if (car_factory) delete car_factory;
  if (config) delete config;

  #ifdef GUI
  deinitAllegro();
  #endif

  if (s != -1) exit(0);
}

/**
 * Nastaveni callbacku pri zachyceni signalu pro zabiti procesu
 */
void setupSignalHandler() {
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = freeMemory;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);
}

CarFactory * getCarFactory(Config * config, int argc, char ** argv) {

  switch(config->getCarFactory()) {
    case 2:
      if (argc >=3) {
        cout << "Loading samples from '" << argv[2] << "'.\n";
        return new CsvCarFactory(argv[2], config);
      } else {
        return new CsvCarFactory(DEAULT_SAMPLES_FILE, config);
      }
    case 3:
      return new NormalCarFactory(config);
    case 1:
    default:
      return new SimpleCarFactory(config);
  }
}

Config * getConfig(int argc, char ** argv) {
  Config * conf = new Config();
  if (argc >= 2) {
    conf->loadFromFile(argv[1]);
    cout << "Settings loaded from '" << argv[1] << "'.\n";
  } else {
    conf->loadFromFile("data/default.config");
  }

  return conf;
}

/**
 * kontrola, zda klavesa byla naposled stisknuta alespon pred 1s
 */
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

#ifdef GUI
void guiLoop(World * world) {

  initAllegro(config->getNumberOfTrackCells());

  BITMAP * buffer2 = create_bitmap(screen_width, screen_height);

  bool running = true, stop = false;
  clear_to_color(screen, makecol(255, 255, 255));

  while (!key[KEY_ESC] && world->isLive()) {

    if (running) {

      /**
       * Krokovani simulace
       */
      world->step();

      int offset = (world->getCurrentTime()*LINE_HEIGHT)%(screen_height-LINE_HEIGHT);

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

      /**
       * Vykresleni cesty
       */
      Cell * cell = world->getTrack()->getFirstCell();
      for (int i = 0; i < world->getTrack()->getLength(); ++i) {
        if (cell->isOccupied()) {
          line(buffer, cell->getPosition(), 0, cell->getPosition(), LINE_HEIGHT, makecol(0,0,0));
        }
        cell = cell->getCellFront();
      }
      stretch_blit(buffer, buffer2, 0, 0, buffer->w, buffer->h, 0, 0, buffer2->w, buffer2->h);
      blit(buffer2, screen, 0, 0, 0, offset, screen_width, LINE_HEIGHT);
    } else {
      rest(40);
    }

    /**
     * Zastaveni simulace pri stistku mezerniku
     */
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
}
#endif

#ifdef ASCII
void asciiLoop(World * world) {
  while (world->isLive()) {

      world->step();
      Cell * cell = world->getTrack()->getFirstCell();

      for (int i = 0; i < world->getTrack()->getLength(); ++i) {
        if (cell->isOccupied()) {
          cout << cell->getCar()->getCurrentSpeed();
        } else {
          cout << '.';
        }
        cell = cell->getCellFront();
      }
      cout << endl;
    }
}
#endif

void loop(World * world) {
    while (world->isLive()) {
      world->step();
    }
}
