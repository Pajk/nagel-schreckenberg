#ifndef _CSV_CAR_FACTORY_H
#define _CSV_CAR_FACTORY_H

#include <fstream>
#include <vector>
#include <string>

#include "car_factory.h"

class Car;

class CsvCarFactory : public CarFactory {

  char column_separator;

  std::ifstream file;

  long current_id;

  Config *config;

  public:

    CsvCarFactory(const char *file_name, Config *config, const char column_separator = ',');

    std::vector<std::string> nextLine();
    
    Car *nextCar();
};

#endif

