#ifndef _DUMMY_CAR_FACTORY_H
#define _DUMMY_CAR_FACTORY_H

#include "car_factory.h"
#include "../car.h"

class DummyCarFactory : public CarFactory {

  public:
    Car * nextCar() { return NULL; }

};

#endif
