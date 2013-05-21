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

CsvCarFactory::CsvCarFactory(const char *file_name, Config *config) {

  this->config = config;
  this->file_name = file_name;
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

  file.close();
}

void CsvCarFactory::resetIterator() {
    current_car = buffer.begin();
    current_id = 1;
}
