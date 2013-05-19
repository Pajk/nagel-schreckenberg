#include <cstdlib>
#include <iostream>
#include <ctime>

#include "track.h"
#include "car.h"
#include "car_factory/car_factory.h"
#include "cell.h"
#include "world.h"
#include "config.h"

Track::Track(Config * config, CarFactory * car_factory, World * world) {

  this->length = config->getNumberOfTrackCells();
  this->periodic_boundary = config->usePeriodicBoundary();
  this->world = world;
  this->car_factory = car_factory;

  last_car = NULL;
  time_offset = 0;

  next_car = car_factory->nextCar();
  if (next_car) {
    time_offset = next_car->getTimeIn();
    next_car->setTimeIn(0);
  }

  createCells();
}

void Track::createCells() {
  Cell *cell = NULL, *cell_tmp = NULL;

  // vytvoreni prvni bunky a ulozeni reference
  cell_tmp = new Cell(0);
  first_cell = cell_tmp;

  // vytvoreni vsech ostatnich bunek a jejich vlozeni do provazaneho seznamu
  for (int i = 1; i < length; ++i) {

    cell = new Cell(i);

    // nastaveni reference na predchozi/nasledujici bunku
    cell->setCellBack(cell_tmp);
    cell_tmp->setCellFront(cell);
    cell_tmp = cell;

    // ulozeni reference na bunku uprostred cesty
    if (i == length/2) {
      middle_cell = cell;
    }
  }

  // ulozeni reference na posledni bunku cesty
  last_cell = cell;

  if (this->periodic_boundary) {
    last_cell->setCellFront(first_cell);
    first_cell->setCellBack(last_cell);
  }
}

Track::~Track() {

  if (next_car) {
    delete next_car;
  }

  if (last_car) {
    delete last_car;
  }

  Cell * cell;
  last_cell->setCellFront(NULL);
  while (first_cell) {
    cell = first_cell;
    first_cell = first_cell->getCellFront();
    if (cell) delete cell;
  }

}

/**
 * Krok simulace
 */
void Track::step() {

  /**
   * Na vozovku vjizdi auto za podminek:
   * - ve fronte je pripraveno auto
   * - toto auto ma vjet v dany cas nebo v casem minulem
   * - na vozovce je dostatek mista pro vjizdejici auto
   */
  if (isEnterAllowed(next_car)) {
    next_car->enterTrack(this);
    // std::cout << world->getCurrentTime() << " " << next_car->getTimeIn() << std::endl;
    last_car = next_car;
    next_car = NULL;
  }

  /**
   * Vykonani jednoho kroku simulace, tzn. vypoctu novych rychlosti a posunu
   * na novou pozici. Jelikoz se zacina u posledniho vozidla a pravidla pro vypocet
   * nove rychlosti nezavisi na vzdalenosti nasledujiciho vozidla (jedouciho
   * za danym vozidlem), dochazi k vypoctu nove rychlosti na zaklade stavu vozovky
   * v predchozim kroku.
   */
  Car * car = last_car;
  Car *tmp_car;
  float mean_speed = 0;
  int cars_count = 0;

  while (car) {
    mean_speed += car->getCurrentSpeed();
    cars_count++;

    tmp_car = car->getCarInFront();
    car->step();
    car = tmp_car;
  }

  // vypocet a zalogovani prumerne rychlosti vsech vozidel na trati
  if (cars_count) {
    mean_speed /= cars_count;
  }
  world->logMeanSpeed(mean_speed);

  /**
   * Pokud neceka na vjeti na vozovku zadne auto, ziska se z car factory
   * nasledujici v poradi. Cas vjeti na vozovku se upravi podle casu, kdy
   * na vozovku vjelo prvni auto, aby cas sedel s casem simulace.
   */
  if (!next_car) {
    next_car = car_factory->nextCar();
    if (next_car) {
      next_car->setTimeIn(next_car->getTimeIn() - time_offset);
    }
  }
}

/**
 * Zkontroluje zda jsou splneny vsechny podminky pro to, aby vozidlo
 * mohlo vjet na vozovku.
 */
bool Track::isEnterAllowed(Car * car) {
  return car &&
    (!last_car || (last_car->getBackPosition() > car->getLength())) &&
    car->getTimeIn() <= world->getCurrentTime();
}

/**
 * Zjisti zda je trat "ziva", tzn zda na ni jsou nejaka vozidla
 * nebo nejaka cekaji ve fronte.
 */
bool Track::isLive() {
  return (next_car || last_car) ? true : false;
}
