#ifndef _SIMPLE_CAR_FACTORY_H
#define _SIMPLE_CAR_FACTORY_H

#include "car_factory.h"
#include "../statistics.h"

class Car;

class SimpleCarFactory : public CarFactory {

  int current_id;
  Config *config;
  Statistics *statistics;

  public:
    SimpleCarFactory(Config *config, Statistics *statistics) {
        this->config = config;
        this->statistics = statistics;
        this->current_id = 1;
    };

    Car *nextCar();

};

#endif

