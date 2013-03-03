#ifndef _SIMPLE_CAR_FACTORY_H
#define _SIMPLE_CAR_FACTORY_H

#include "car_factory.h"

class Car;

class SimpleCarFactory : public CarFactory {

  int current_id;

  public:
    SimpleCarFactory() : current_id(1) {};

    Car *nextCar();

};

#endif

