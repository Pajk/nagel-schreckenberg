#include <iostream>
#include <cstdlib>

#include "car.h"

#ifdef GUI
#include <allegro.h>

extern BITMAP *buffer;
#endif

Car::Car(long id, int car_class) {

  this->id = id;
  this->car_class = car_class;

  this->car_in_front = NULL;
  this->car_behind = NULL;
  this->track = NULL;
  this->expected_time = 0;
  this->position = 0;
  this->old_position = 0;
  this->time_in = 0;
  this->current_speed = 0;

  // motocykl
  if (car_class == 0) {
    this->max_speed = 6;
    this->p = 0.3;
    this->current_speed = 5;
  // osobni automobil do 3.5 tuny
  } else if (car_class == 1) {
    this->max_speed = 5;
    this->p = 0.3;
    this->current_speed = 3;
  // dodavka do 3.5 tuny
  } else if (car_class == 2) {
    this->max_speed = 4;
    this->p = 0.5;
    this->current_speed = 2;
  // autobus
  } else if (car_class == 3) {
    this->max_speed = 4;
    this->p = 0.5;
    this->current_speed = 2;
  // nakladni vozidlo do 6 tun
  } else if (car_class == 4) {
    this->max_speed = 3;
    this->p = 0.3;
    this->current_speed = 1;
  // nakladni vozidlo na 6 tun
  } else if (car_class == 5) {
    this->max_speed = 3;
    this->p = 0.5;
    this->current_speed = 1;
  } else {
    this->max_speed = 5;
    this->p = 0.3;
    this->current_speed = 2;
  }
}

Car::~Car() {
      
  int total_time = 0;
  if (track) {
    total_time = track->getCurrentTime() - time_in;
  }

  //std::cout << "auto " << id << " konci po " << total_time << 
  //  " na pozici " << position << " s rychlosti " << current_speed << std::endl;
  std::cout << id << " " << total_time - expected_time << std::endl;
  
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
  if (current_speed < max_speed && dist > current_speed + 1) {
    current_speed++;
  }

  // 2) zpomaleni
  if (current_speed >= dist) {
    current_speed = dist - 1;
  }

  // 3) randomizace
  if ((rand() < RAND_MAX*p) && (current_speed > 0)) {
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

