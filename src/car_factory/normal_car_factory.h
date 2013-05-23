/*
 * Datum:   20.5.2013
 * Autor:   Pavel Pokorny, xpokor12@stud.fit.vutbr.cz
 * Projekt: DP - Kalibrace mikrosimulacniho modelu dopravy
 */

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

  double V1, V2, S;
  int phase;

  public:
    NormalCarFactory(Config *config) {
      setConfig(config);
    };

    void setConfig(Config * config) {
      this->config = config;
      if (config != NULL) {
        current_id = 1;
        phase = 0;
        last_car_time = 0;
        car_types = config->getNumberOfCarTypes();
        mean = config->getNCFMean();
        deviation = config->getNCFDeviation();
      }
    }

    Car *nextCar();

  protected:
    double gaussrand();
};

#endif
