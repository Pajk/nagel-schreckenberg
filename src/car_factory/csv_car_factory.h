#ifndef _CSV_CAR_FACTORY_H
#define _CSV_CAR_FACTORY_H

#include <fstream>
#include <vector>
#include <string>

#include "car_factory.h"
#include "../statistics.h"

class Car;

class CsvCarFactory : public CarFactory {

  char column_separator;

  long current_id;

  const char *file_name;

  Config *config;

  Statistics *statistics;

  std::vector<std::vector<std::string> > buffer;

  std::vector<std::vector<std::string> >::iterator current_car;

  public:

    CsvCarFactory(const char *file_name, Config *config = NULL, Statistics *statistics = NULL);
    virtual ~CsvCarFactory();

    std::vector<std::string> nextLine();

    Car *nextCar();

    void fillBuffer();

    void resetIterator();
    void sortByTimeIn();
    void sortByExpectedTimeOut();

    void setConfig(Config *config) { this->config = config; }
    void setStatistics(Statistics *statistics) { this->statistics = statistics; }
};

#endif

