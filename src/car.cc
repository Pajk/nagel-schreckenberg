#include <iostream>
#include <cstdlib>

#include "car.h"
#include "cell.h"
#include "world.h"
#include "track.h"

using std::cout;
using std::endl;

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
  if (config->getSlowToStop()) {
    slowToStopStep();
  } else {
    basicStep();
  }
}

void Car::slowToStopStep() {

  old_position = position;

  int free_cells = getFreeCellsCount(max_speed * 2);
  bool speed_modified = false;

  // ziskani rychlosti predchoziho auta, pokud zadne neni v dohlednu, vrati -1
  int v_next = getCarAheadSpeed(free_cells);

  /**
   * 1) slow to start
   * Pokud v = 0 a d > 1, pak s pravdepodobností 1 - p_ zrychli vozidlo normalne
   * (tento krok je ignorovan) a s pravdepodobností pslow zustane rychlost vozidla
   * v tomto kroku nulova a vozidlo zrychli na v = 1 v nasledujicim kroku simulace
   */
  if (slow_start) {
    current_speed = 1;
    speed_modified = true;
    slow_start = false;
  } else if (current_speed == 0 && free_cells > 1 &&
    rand() < RAND_MAX*config->getSlowToStartProbability()) {
    slow_start = true;
  }

  /**
   * 2) zpomaleni, pokud jedu blizko predchoziho vozidla
   */
  if (free_cells <= current_speed) {
    if ((current_speed < v_next) || (current_speed <= 2)) {
      current_speed = free_cells - 1;
    } else {
      current_speed = (free_cells - 1 < current_speed - 1 ? free_cells - 1 : current_speed - 1);
    }
    speed_modified = true;
  }

  /**
   * 3) zpomaleni, pokud je predchozi vozidlo ve vetsi vzdalenosti
   */
  if (current_speed < free_cells && free_cells <= 2 * current_speed) {
    if (current_speed >= v_next + 4) {
      current_speed -= 2;
      speed_modified = true;
    } else if (v_next + 2 <= current_speed && current_speed <= v_next + 3) {
      current_speed -= 1;
      speed_modified = true;
    }
  }

  /**
   * 4) Zrychleni
   */
  if (slow_start == false && speed_modified == false) {
    if (current_speed < max_speed && free_cells > current_speed) {
      current_speed++;
    }
  }

  /**
   * 5) nahodne zpomaleni
   */
  if (rand() < RAND_MAX*slowdown_probability && current_speed > 0) {
    current_speed--;
  }

  // 6) pohyb auta
  // std::cout << id << " rychlost " << current_speed << " = " << current_speed * config->getSiteLength() * 3.6 << " km/h" << std::endl;
  advanceCells(current_speed);
}

void Car::basicStep() {

  old_position = position;
  int free_cells = getFreeCellsCount(max_speed);

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

int Car::getCarAheadSpeed(int free_cells) {

  if (cell == NULL) {
    return max_speed * 2;
  }

  Cell * tmp = cell->getCellFront();

  for (int i = 0; tmp != NULL && i < free_cells; ++i) {
    tmp = tmp->getCellFront();
  }

  if (tmp == NULL || tmp->getCar() == NULL) {
    return -1;
  } else {
    return tmp->getCar()->getCurrentSpeed();
  }
}

int Car::getFreeCellsCount(int visibility) {

  // zjiteni poctu volnych bunek pred vozidlem
  // zkontroluje se tolik bunek pred vozidlem, jaka je
  // jeho maximalni rychlost

  // pokud predchozi vozidlo vyjelo uz z vozovky, vrati
  // se delka cele vozovky - je zapnuty
  if (cell == NULL) {
    return track->getLength();
  }

  Cell * tmp = cell->getCellFront();
  int free_cells = 0;

  for (int i = 0;
       tmp != NULL && tmp->isEmpty() && i < visibility;
       tmp = tmp->getCellFront(), ++i) {
    free_cells++;
  }

  // pokud nejsou pouzity periodic boundary conditions
  // je treba zkontrolovat, jestli neni vozidlo na konci trasy
  // pokud ano, vrati se pocet o jednu bunku vetsi, aby bylo
  // vozidlo schopno vyjet z vozovky
  if (tmp == NULL) {
    free_cells = track->getLength();
  }

  return free_cells;
}
