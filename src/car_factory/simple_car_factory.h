#ifndef _SIMPLE_CAR_FACTORY_H
#define _SIMPLE_CAR_FACTORY_H

#include "car_factory.h"

class Car;

class SimpleCarFactory : public CarFactory {

  int current_id;
  long last_car_time;
  int car_types;
  Config *config;

  public:
    SimpleCarFactory(Config *config) : config(config) {
        current_id = 1;
        last_car_time = 0;
        int car_types = config->getNumberOfCarTypes();
        if (car_types > 1) car_types--;
    };

    Car *nextCar();

};

#endif

