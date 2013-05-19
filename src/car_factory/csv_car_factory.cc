#include <cstdlib>
#include <sstream>
#include <iterator>
#include <ctime>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "csv_car_factory.h"
#include "../car.h"

using namespace std;

/**
 * Pomocna funkce pro zpracovani retezce s casem
 */
time_t parseTime(const char * time_string) {

  struct tm tm_struct;
  strptime(time_string, "%d-%m-%Y %H:%M:%S", &tm_struct);
  tm_struct.tm_isdst = -1;

  return mktime(&tm_struct);
}

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

    int car_class = atoi(line.at(5).c_str());

    Car *car = new Car(current_id++, car_class, config);

    time_t time_in = parseTime(line.at(2).c_str());

    if (time_in == -1) {
      cerr << "Error while parsing time in.\n";
    }

    car->setTimeIn((long)time_in);

    int expected_time = atoi(line.at(3).c_str());

    car->setExpectedTime(expected_time);

    return car;

  } else {

    return NULL;
  }
}

CsvCarFactory::CsvCarFactory(const char *file_name, Config *config) {

  this->column_separator = column_separator;
  this->config = config;
  this->column_separator = ',';
  this->file_name = file_name;

  fillBuffer();
  resetIterator();
}

void CsvCarFactory::fillBuffer() {

  ifstream file(file_name, ifstream::in);

  if (!file.good()) {
    cout << "Cannot open samples file '" << file_name << "'." << endl;
    exit(-1);
  }

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

  sortByTimeIn();

  file.close();
}

struct less_than_time_in {
    inline bool operator() (const vector<string>& v1, const vector<string>& v2) {

      time_t v1_time_in, v2_time_in;

      v1_time_in = parseTime(v1.at(2).c_str());
      v2_time_in = parseTime(v2.at(2).c_str());

      if (v1_time_in == -1 || v2_time_in == -1) {
        cerr << "Error while parsing time in.\n";
      }

      return v1_time_in < v2_time_in;
    }
};

struct less_than_time_out {
    inline bool operator() (const vector<string>& v1, const vector<string>& v2) {

      time_t v1_time_out, v2_time_out;

      v1_time_out = parseTime(v1.at(1).c_str());
      v2_time_out = parseTime(v2.at(1).c_str());

      if (v1_time_out == -1 || v2_time_out == -1) {
        cerr << "Error while parsing time in.\n";
      }

      return v1_time_out < v2_time_out;
    }
};

void CsvCarFactory::sortByTimeIn() {
  sort(buffer.begin(), buffer.end(), less_than_time_in());
  resetIterator();
}

void CsvCarFactory::sortByExpectedTimeOut() {
  sort(buffer.begin(), buffer.end(), less_than_time_out());
  resetIterator();
}

CsvCarFactory::~CsvCarFactory() {
}

void CsvCarFactory::resetIterator() {
    current_car = buffer.begin();
    current_id = 1;
}
