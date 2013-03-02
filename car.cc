#include <iostream>
#include <cstdlib>
#include <allegro.h>

#include "car.h"

extern BITMAP *buffer;

Car::Car(int id, int car_class, Car *car_in_front, Track *track) {

  this->id = id;
  this->car_in_front = car_in_front;
  this->car_behind = NULL;
  this->exp_time = 0;
  this->car_class = car_class;
  this->position = 0;
  this->old_position = 0;
  this->in_time = track->getCurrentTime();
  this->track = track;
  this->cur_speed = 0;

  if (car_in_front) {
    car_in_front->setCarBehind(this);
  }

  if (car_class == 0) {
    this->max_speed = 5;
    this->p = 0.3;
    this->cur_speed = 3;
  } else if (car_class == 1) {
    this->max_speed = 4;
    this->p = 0.7;
    this->cur_speed = 3;
  } else if (car_class == 2) {
    this->max_speed = 7;
    this->p = 0.2;
    this->cur_speed = 4;
  }
}

Car::~Car() {
      
  std::cout << "auto " << id << " konci po " << track->getCurrentTime() - in_time << 
    " na pozici " << position << " s rychlosti " << cur_speed << std::endl;
  
  if (car_in_front) {
    car_in_front->setCarBehind(NULL);
  }
  
  if (car_behind) {
    car_behind->setCarInFront(NULL);
  }

  if (track->getLeftCar() == this) {
    track->setLeftCar(NULL);
  }
}

void Car::move() {

  old_position = position;

  if (cur_speed < max_speed) {
    cur_speed++;
  }

  int dist = getDistance();
  if (cur_speed >= dist) {
    cur_speed = dist - 1;
  }

  if ((rand() < RAND_MAX*p) && (cur_speed > 0)) {
    cur_speed--;
  }

  position += cur_speed;

  if (position > track->getLength()) {
    delete this;
  } else {
    line(buffer, position, 0, position, 50, makecol(255,0,0));
  }
}

