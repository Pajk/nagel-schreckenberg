/*
 * Datum:   20.5.2013
 * Autor:   Pavel Pokorny, xpokor12@stud.fit.vutbr.cz
 * Projekt: DP - Kalibrace mikrosimulacniho modelu dopravy
 */

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
    SimpleCarFactory(Config *config) {
      setConfig(config);
    };

    void setConfig(Config * config) {
      this->config = config;
      if (config != NULL) {
        current_id = 1;
        last_car_time = 0;
        car_types = config->getNumberOfCarTypes();
      }
    }

    Car *nextCar();
};

#endif

