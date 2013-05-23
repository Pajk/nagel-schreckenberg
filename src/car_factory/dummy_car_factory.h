/*
 * Datum:   20.5.2013
 * Autor:   Pavel Pokorny, xpokor12@stud.fit.vutbr.cz
 * Projekt: DP - Kalibrace mikrosimulacniho modelu dopravy
 */

#ifndef _DUMMY_CAR_FACTORY_H
#define _DUMMY_CAR_FACTORY_H

#include "car_factory.h"
#include "../car.h"

class DummyCarFactory : public CarFactory {

  public:
    Car * nextCar() { return NULL; }

    void setConfig(Config * config) {
        this->config = config;
    }

};

#endif
