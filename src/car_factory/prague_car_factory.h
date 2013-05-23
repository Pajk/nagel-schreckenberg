/*
 * Datum:   20.5.2013
 * Autor:   Pavel Pokorny, xpokor12@stud.fit.vutbr.cz
 * Projekt: DP - Kalibrace mikrosimulacniho modelu dopravy
 */

#ifndef _PRAGUE_CAR_FACTORY_H
#define _PRAGUE_CAR_FACTORY_H

#include "csv_car_factory.h"
#include <iostream>

class PragueCarFactory : public CsvCarFactory {

  public:

    PragueCarFactory(const char *file_name, Config *config) :
      CsvCarFactory(file_name, config) {

      column_separator = ',';

      fillBuffer();
      setConfig(config);
    }

    Car *nextCar();

    void sortByTimeIn();

    void sortByExpectedTimeOut();

    void setConfig(Config * config) {
      this->config = config;

      if (config != NULL) {
        resetIterator();
        sortByTimeIn();
      }
    }
};

#endif

