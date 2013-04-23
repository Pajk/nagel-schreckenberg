#include <iostream>
#include <cstdlib>

#include "car.h"

#ifdef GUI
#include <allegro.h>

extern BITMAP *buffer;
#endif

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

  loadCarConfig();
}

void Car::loadCarConfig() {

  CarConfig c = config->getCarConfig(car_class);

  slowdown_probability = c.slowdown_probability;
  acceleration_probability = c.acceleration_probability;
  max_speed = c.max_speed;
  min_speed = c.min_speed;
  current_speed = (max_speed - min_speed)/2;
}

Car::~Car() {
  
  int total_time = 0;
  if (track) {
    total_time = track->getCurrentTime() - time_in;
  }

  //std::cout << "auto " << id << " konci po " << total_time << 
  //  " na pozici " << position << " s rychlosti " << current_speed << std::endl;
  // std::cout << id << " " << total_time << " " << expected_time << " " << total_time - expected_time << std::endl;

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
  if (rand() < RAND_MAX*slowdown_probability && current_speed > 0) {
    current_speed--;
  }

  // 4) pohyb auta
  position += current_speed;

  if (position > track->getLength()) {
    delete this;
  #ifdef GUI
  } else {
    line(buffer, position, 0, position, 50, makecol(255,0,0));
  #endif
  }
}

void Car::start(Track *track, Car *car_in_front) {
  
  this->track = track;
  this->car_in_front = car_in_front;
  this->time_in = track->getCurrentTime();
  
  if (car_in_front) {
    car_in_front->setCarBehind(this);
  }

}

