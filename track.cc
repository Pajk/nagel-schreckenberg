#include <cstdlib>
#include <iostream>

#include "track.h"
#include "car.h"

Track::Track(int length) {
  
  this->length = length;
  this->number_of_cars = 0;
  this->sim_time = 0;
  this->left_car = NULL;
  this->car = NULL;  
}

Track::~Track() {
  if (left_car) {
    delete left_car;
  }
}

void Track::step() {
  
  if (!left_car || (left_car->getPosition() > 2)) {
    left_car = new Car(++number_of_cars, rand()%3, left_car, this);
  }

  car = left_car;

  while (car) {
    car->move();
    car = car->getCarInFront();
  }

  sim_time++;
}

