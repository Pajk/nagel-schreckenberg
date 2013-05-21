#ifndef _CAR_FACTORY_H
#define _CAR_FACTORY_H

#include "../config.h"

class Car;

class CarFactory {

  protected:

    Config * config;

  public:

    virtual Car *nextCar() = 0;
    virtual ~CarFactory() {};

    virtual void setConfig(Config * config) = 0;
};

#endif

