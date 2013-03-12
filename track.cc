#include <cstdlib>
#include <iostream>
#include <ctime>

#include "track.h"
#include "car.h"
#include "car_factory.h"

Track::Track(CarFactory *car_factory, int length) {
  
  this->length = length;
  this->number_of_cars = 0;
  this->sim_time = 0;
  this->last_car = NULL;
  this->car = NULL;
  this->car_factory = car_factory;
  this->start_time = NULL;
  this->next_car = NULL;
  
  next_car = car_factory->nextCar();
  time_offset = next_car->getTimeIn();
  next_car->setTimeIn(0);
}

Track::~Track() {
 
  if (next_car) {
    delete next_car;
  }
 
  if (last_car) {
    delete last_car;
  }

  if (car_factory) {
    delete car_factory;
  }

}

void Track::step() {
  
  if (next_car && (!last_car || (last_car->getPosition() > 2))) {
    
    if (next_car->getTimeIn() <= sim_time) {
      next_car->start(this, last_car);
      last_car = next_car;
      next_car = NULL;
      number_of_cars++;
    }
  }

  car = last_car;

  while (car) {
    car->move();
    car = car->getCarInFront();
  }

  sim_time += 1;
  
  if (!next_car) {
    next_car = car_factory->nextCar();
    if (next_car) {
      next_car->setTimeIn(next_car->getTimeIn() - time_offset);
    }
  }
}

bool Track::live() {
  
  return (next_car || last_car) ? true : false;
}

