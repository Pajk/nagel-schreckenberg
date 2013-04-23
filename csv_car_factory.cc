#include <cstdlib>
#include <sstream>
#include <iterator>
#include <ctime>
#include <iostream>
#include <fstream>

#include "csv_car_factory.h"
#include "car.h"

using namespace std;

vector<string> CsvCarFactory::nextLine() {

  if (current_car == buffer.end()) {
    vector<string> cd;
    return cd;
  }

  vector<string> car_data = *current_car;

  current_car++;
  
  return car_data;
}

Car *CsvCarFactory::nextCar() {

  // 0:data-type, 1:time out, 2:time in, 3:total time, 4:car type in, 5:car type out
  // EVCEO-EVVN,22-06-2012 04:51:16,22-06-2012 04:43:34,462,-1,2

  std::vector<std::string> line = nextLine();

  if (line.size() == 6 && line.at(0) == "EVCEO-EVVN") {
    
    struct tm tm_in;
    time_t time_in;

    int car_class = atoi(line.at(5).c_str());

    Car *car = new Car(current_id++, car_class, config, statistics);
    
    strptime(line.at(2).c_str(), "%d-%m-%Y %H:%M:%S", &tm_in);

    tm_in.tm_isdst = -1;
    
    time_in = mktime(&tm_in);

    if (time_in == -1) {
      std::cout << "Chyba pri cteni datumu" << std::endl;
    }
  
    car->setTimeIn((long)time_in);

    car->setExpectedTime(atoi(line.at(3).c_str()));

    return car;

  } else {
    
    return NULL;
  }
}

CsvCarFactory::CsvCarFactory(const char *file_name, Config *config, Statistics *statistics) {
  
  this->column_separator = column_separator;
  this->config = config;
  this->statistics = statistics;
  this->column_separator = ',';
  this->file_name = file_name;

  fillBuffer();
  resetIterator();
}

void CsvCarFactory::fillBuffer() {

  ifstream file(file_name, ifstream::in);

  string line;

  while(getline(file, line)) {

    vector<std::string> chunks;

    stringstream lineStream(line);
    string cell;

    while(getline(lineStream, cell, column_separator)) {
        chunks.push_back(cell);
    }

    buffer.push_back(chunks);
  }

  file.close();
}

CsvCarFactory::~CsvCarFactory() {
}

void CsvCarFactory::resetIterator() {
    current_car = buffer.begin();
    current_id = 1;
}
