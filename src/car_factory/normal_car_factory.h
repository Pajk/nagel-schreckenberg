#ifndef _NORMAL_CAR_FACTORY_H
#define _NORMAL_CAR_FACTORY_H

#include "car_factory.h"

class Car;

class NormalCarFactory : public CarFactory {

  int current_id;
  long last_car_time;
  int car_types;
  int mean;
  float deviation;
  Config *config;

  double V1, V2, S;
  int phase;

  public:
    NormalCarFactory(Config *config) : config(config) {
        current_id = 1;
        last_car_time = 0;
        int car_types = config->getNumberOfCarTypes();
        if (car_types > 1) car_types--;

        phase = 0;
        mean = config->getNCFMean();
        deviation = config->getNCFDeviation();
    };

    Car *nextCar();

  protected:
    double gaussrand();
};

#endif
