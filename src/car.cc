#include <iostream>
#include <cstdlib>

#include "car.h"
#include "cell.h"
#include "world.h"
#include "track.h"

Car::Car(long id, int car_class, Config *config) {

  this->id = id;
  this->car_class = car_class;
  this->config = config;

  this->car_in_front = NULL;
  this->car_behind = NULL;
  this->track = NULL;
  this->expected_time = 0;
  this->position = 0;
  this->old_position = 0;
  this->time_in = 0;
  this->length = 1;

  loadCarConfig();
}

void Car::loadCarConfig() {

  CarConfig c = config->getCarConfig(car_class);

  // c.print();

  slowdown_probability = c.slowdown_probability;
  acceleration_probability = c.acceleration_probability;
  max_speed = c.max_speed;
  min_speed = c.min_speed;
  current_speed = max_speed;
  length = c.length;
}

Car::~Car() {

  // std::cout << "auto " << id << " konci po " << total_time
  //           << " (" << expected_time << ")"
  //           << " s rychlosti " << current_speed
  //           << " a chybou " << expected_time - total_time
  //           << std::endl;

  if (car_in_front) {
    car_in_front->setCarBehind(NULL);
    delete car_in_front;
  }

  if (car_behind) {
    car_behind->setCarInFront(NULL);
  }

  if (track) {
    if (track->getLastCar() == this) {
      track->setLastCar(NULL);
    }
  }
}

void Car::step() {

  old_position = position;
  int free_cells = getFreeCellsCount();

  // 1) akceleracce
  if (rand() < RAND_MAX*acceleration_probability && current_speed < max_speed
    && free_cells > current_speed) {

    current_speed++;
  }

  // 2) pokud je predchozi auto vzdalene mene nez je aktualni rychlost, vozidlo
  // zpomali tak, aby nedoslo ke srazce
  if (current_speed > free_cells) {
    current_speed = free_cells;
  }

  // 3) randomizace - zpomaleni
  if (rand() < RAND_MAX*slowdown_probability && current_speed > min_speed) {
    current_speed--;
  }

  // 4) pohyb auta
  // std::cout << id << " rychlost " << current_speed << " = " << current_speed * config->getSiteLength() * 3.6 << " km/h" << std::endl;
  advanceCells(current_speed);

}

void Car::enterTrack(Track *track) {

  this->track = track;
  this->car_in_front = track->getLastCar();
  this->cell = track->getFirstCell();
  this->time_in = track->getWorld()->getCurrentTime();

  // posune auto o pocet bunek dopredu
  advanceCells(length);

  if (car_in_front) {
    car_in_front->setCarBehind(this);
  }

}

void Car::advanceCells(int cells) {

  position += cells;
  Cell * tmp;
  int i;

  // uvolneni vsech bunek, ktere auto zabira
  for (i = 0, tmp = cell; tmp && i < length; ++i) {
    tmp->free();
    tmp = tmp->getCellBack();
  }

  if (track->hasPeriodicBoundary()) {
    if (position >= track->getLength()) {
      position %= track->getLength();
      track->getWorld()->logStats(this);
    }
  } else {
    // pokud auto vyjelo z vozovky, zaradime ho do seznamu vozidel ke smazani
    if (position >= track->getLength()) {
      track->getWorld()->addOutOfTrackCar(this);
      return;
    }
  }

  // nalezeni bunky, na kterou se ma auto presunout
  for (int i = 0; cell && i < cells; ++i) {
    cell = cell->getCellFront();
  }

  // obsazeni vsech bunek, ktere auto ma zabirat
  for (i = 0, tmp = cell; tmp && i < length; ++i) {
    tmp->setCar(this);
    tmp = tmp->getCellBack();
  }
}

int Car::getFreeCellsCount() {

  // zjiteni poctu volnych bunek pred vozidlem
  // zkontroluje se tolik bunek pred vozidlem, jaka je
  // jeho maximalni rychlost
  Cell * tmp = cell->getCellFront();
  int free_cells = 0;

  for (int i = 0;
       tmp && tmp->isEmpty() && i < max_speed;
       tmp = tmp->getCellFront(), ++i) {
    free_cells++;
  }

  // pokud nejsou pouzity periodic boundary conditions
  // je treba zkontrolovat, jestli neni vozidlo na konci trasy
  // pokud ano, vrati se pocet o jednu bunku vetsi, aby bylo
  // vozidlo schopno vyjet z vozovky
  if (!tmp) {
    free_cells++;
  }

  return free_cells;
}
