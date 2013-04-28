#include <cstdlib>
#include <iostream>
#include <ctime>

#include "track.h"
#include "car.h"
#include "car_factory/car_factory.h"
#include "cell.h"

#ifdef GUI
#include <allegro.h>

extern BITMAP *buffer;
#endif

void Track::createCells() {
  Cell *cell = NULL, *cell_tmp = NULL;

  // vytvoreni prvni bunky a ulozeni reference
  cell_tmp = new Cell(0);
  first_cell = cell_tmp;

  for (int i = 1; i < length; ++i) {

    cell = new Cell(i);

    // nastaveni reference na predchozi/nasledujici bunku
    cell->setCellBack(cell_tmp);
    cell_tmp->setCellFront(cell);
    cell_tmp = cell;
  }

  // ulozeni reference na posledni bunku cesty
  last_cell = cell;
}

Track::Track(CarFactory *car_factory, int length, int max_speed) {

  this->length = length;
  number_of_cars = 0;
  sim_time = 0;
  last_car = NULL;
  car = NULL;
  this->car_factory = car_factory;
  start_time = NULL;
  time_offset = 0;
  delete_list = NULL;
  this->max_speed = max_speed;

  next_car = car_factory->nextCar();
  if (next_car) {
    time_offset = next_car->getTimeIn();
    next_car->setTimeIn(0);
  }

  createCells();
}

Track::~Track() {

  if (next_car) {
    delete next_car;
  }

  if (last_car) {
    delete last_car;
  }

  Cell * cell;
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
    next_car->start(this, last_car, first_cell);
    last_car = next_car;
    next_car = NULL;
    number_of_cars++;
  }

  car = last_car;

  /**
   * Vykonani jednoho kroku simulace.
   */
  Car *tmp_car;
  while (car) {
    tmp_car = car->getCarInFront();
    car->move();
    car = tmp_car;
  }

  sim_time += 1;

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

  /**
   * Vykresleni cesty
   */
  #ifdef GUI
    Cell * tmp = first_cell;
    while (tmp) {
      // std::cout << "vykresluju " << tmp->getPosition() << " - " << tmp->isOccupied() << std::endl;
      if (tmp->isOccupied()) {
        line(buffer, tmp->getPosition(), 0, tmp->getPosition(), 10, makecol(255,0,0));
      }
      tmp = tmp->getCellFront();
    }
  #endif

  /**
   * Smazani vozidel, ktra vyjela z vozovky
   */
  if (delete_list) {
    delete delete_list;
    delete_list = NULL;
  }
}

bool Track::isEnterAllowed(Car * car) {
  return car &&
    (!last_car || (last_car->getBackPosition() > car->getLength())) &&
    car->getTimeIn() <= sim_time;
}

bool Track::isLive() {

  return (next_car || last_car) ? true : false;
}

/**
 * Prida auto do seznamu aut pro smazani
 */
void Track::addOutOfTrackCar(Car * car) {
  if (delete_list) {
    delete_list->setCarBehind(car);
  }
  car->setCarInFront(delete_list);
  delete_list = car;

  if (car->getCarBehind()) {
    car->getCarBehind()->setCarInFront(NULL);
  }
}

