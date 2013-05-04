#include <iostream>
#include <cstdlib>

#include "car.h"
#include "cell.h"

Car::Car(long id, int car_class, Config *config, Statistics *statistics) {

  this->id = id;
  this->car_class = car_class;
  this->config = config;
  this->statistics = statistics;

  this->car_in_front = NULL;
  this->car_behind = NULL;
  this->track = NULL;
  this->expected_time = 0;
  this->position = 0;
  this->old_position = 0;
  this->time_in = 0;
  this->current_speed = 0;
  this->length = 1;

  loadCarConfig();
}

void Car::loadCarConfig() {

  CarConfig c = config->getCarConfig(car_class);

  slowdown_probability = c.slowdown_probability;
  acceleration_probability = c.acceleration_probability;
  max_speed = c.max_speed;
  min_speed = c.min_speed;
  current_speed = (max_speed - min_speed)/2;
  length = c.length;
}

Car::~Car() {

  int total_time = 0;
  if (track) {
    total_time = track->getCurrentTime() - time_in;
  }

  // std::cout << "auto " << id << " konci po " << total_time << " ("
  //           << expected_time << ")" << " s rychlosti " << current_speed << std::endl;

  statistics->logCarTime(id, total_time, expected_time);

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

void Car::move() {

  old_position = position;
  int dist = getDistance();

  /**
   * Pokud je maximalni povolena rychlost na vozovce mensi nez maximalni
   * mozna rychlost vozidla, bere se ohled na na max. povolenou rychlost.
   */
  // int max_allowed_speed = track->getMaxSpeed() < max_speed ? track->getMaxSpeed() : max_speed;

  // 1) akceleracce
  if (rand() < RAND_MAX*acceleration_probability && current_speed < max_speed
    && dist > current_speed + 1) {

    current_speed++;
  }

  // 2) zpomaleni
  if (current_speed >= dist) {
    current_speed = dist - 1;
  }

  // 3) randomizace - zpomaleni
  if (rand() < RAND_MAX*slowdown_probability && current_speed > min_speed) {
    current_speed--;
  }

  // 4) pohyb auta
  // std::cout << id << " rychlost " << current_speed << " = " << current_speed * config->getSiteLength() * 3.6 << " km/h" << std::endl;
  advanceCells(current_speed);

}

void Car::start(Track *track, Car *car_in_front, Cell *first_cell) {

  this->track = track;
  this->car_in_front = car_in_front;
  this->time_in = track->getCurrentTime();
  this->cell = first_cell;

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

  // pokud auto vyjelo z vozovky, zaradime ho do seznamu vozidel ke smazani
  if (position >= track->getLength()) {
    track->addOutOfTrackCar(this);
    return;
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

