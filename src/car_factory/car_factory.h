#ifndef _CAR_FACTORY_H
#define _CAR_FACTORY_H

#include "../config.h"

class Car;

class CarFactory {

  public:

    virtual Car *nextCar() = 0;
    virtual ~CarFactory() {};

};

#endif

