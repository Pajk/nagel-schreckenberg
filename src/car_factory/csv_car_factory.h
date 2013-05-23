/*
 * Datum:   20.5.2013
 * Autor:   Pavel Pokorny, xpokor12@stud.fit.vutbr.cz
 * Projekt: DP - Kalibrace mikrosimulacniho modelu dopravy
 */

#ifndef _CSV_CAR_FACTORY_H
#define _CSV_CAR_FACTORY_H

#include <fstream>
#include <vector>
#include <string>

#include "car_factory.h"

class Car;

class CsvCarFactory : public CarFactory {

  public:

    CsvCarFactory(const char *file_name, Config *config);

    std::vector<std::string> nextLine();

    void fillBuffer();

    void resetIterator();

    virtual void setConfig(Config *config) = 0;

  protected:

    char column_separator;
    long current_id;
    const char *file_name;
    Config *config;
    std::vector<std::vector<std::string> > buffer;
    std::vector<std::vector<std::string> >::iterator current_car;
};

#endif

