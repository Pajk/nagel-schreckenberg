#ifndef _SIMPLE_CAR_FACTORY_H
#define _SIMPLE_CAR_FACTORY_H

#include "car_factory.h"

class Car;

class SimpleCarFactory : public CarFactory {

  int current_id;
  Config *config;

  public:
    SimpleCarFactory(Config *config) : current_id(1), config(config) {};

    Car *nextCar();

};

#endif

