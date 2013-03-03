#ifndef _CAR_FACTORY_H
#define _CAR_FACTORY_H

class Car;

class CarFactory {

  public:

    virtual Car *nextCar() = 0;

};

#endif

