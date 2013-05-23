#include <cstdlib>
#include <cmath>
#include <sstream>
#include <iterator>
#include <ctime>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "seattle_car_factory.h"
#include "../car.h"

using namespace std;

namespace Seattle {

  /**
   * Pomocna funkce pro zpracovani retezce s casem
   */
  time_t parseTime(const char * time_string) {

    struct tm tm_struct;
    strptime(time_string, "\"%Y-%d-%m %H:%M:%S\"", &tm_struct);
    tm_struct.tm_isdst = -1;

    return mktime(&tm_struct);
  }

}

double SeattleCarFactory::gaussrand() {

    double X;

    if(phase == 0) {
        do {
            double U1 = (double)rand() / RAND_MAX;
            double U2 = (double)rand() / RAND_MAX;

            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
            } while(S >= 1 || S == 0);

        X = V1 * sqrt(-2 * log(S) / S);
    } else
        X = V2 * sqrt(-2 * log(S) / S);

    phase = 1 - phase;

    // std::cout << phase << " " << X << "*" << deviation << "+" << mean << "=";
    X = X * deviation + mean;

    // nechceme zaporna cisla
    if (X < 0) X = 0;

    return X;
}

void SeattleCarFactory::getNextInterval() {
  cout << "interval: ";
  //"Time";"TravelTime";"X1110.intensity";"X1110.occupancy";"X921.intensity";"X921.occupancy";"X1071.intensity";"X1071.occupancy";"X876.intensity";"X876.occupancy";"X740.intensity";"X740.occupancy";"X1056.intensity";"X1056.occupancy";"X1049.intensity";"X1049.occupancy";"X862.intensity";"X862.occupancy";"X1230.intensity";"X1230.occupancy";"X764.intensity";"X764.occupancy";"X907.intensity";"X907.occupancy";"X941.intensity";"X941.occupancy";"X1218.intensity";"X1218.occupancy";"X1035.intensity";"X1035.occupancy";"X896.intensity";"X896.occupancy";"X1085.intensity";"X1085.occupancy";"X850.intensity";"X850.occupancy";"X952.intensity";"X952.occupancy";"X404.intensity";"X404.occupancy";"X384.intensity";"X384.occupancy"
  //"2011-09-01 00:05:00";1331.4;41;43;29;25;37;29;30;28;21;17;38;30;36;25;31;28;51;49;18;14;28;25;31;26;40;36;36;25;29;41;30;21;16;12;28;27;26;26;26;27
  vector<string> line = nextLine();
  if (int(line.size()) > intensity_column && int(line.size()) > occupancy_column) {
    cout << line.at(0);
    interval_time = Seattle::parseTime(line.at(time_column).c_str());

    if (interval_time == -1) {
      cerr << "Error while parsing interval time.\n";
      exit(-1);
    }

    interval_travel_time = atof(line.at(travel_time_column).c_str());
    interval_intensity = atoi(line.at(intensity_column).c_str());
    interval_occupancy = atoi(line.at(occupancy_column).c_str());
    // vypocet novych parametru pro generator nahodnych cisel
    mean = float(config->getStatsFrequency()) /  interval_intensity;
    deviation = mean * 0.5; // 50%
    last_car_time = interval_time;

  } else {
    interval_time = 0;
    interval_travel_time = 0;
    interval_intensity = 0;
    interval_occupancy = 0;
  }
  cout << endl;
}

Car *SeattleCarFactory::nextCar() {

  if (interval_time == 0
      || last_car_time >= interval_time + config->getStatsFrequency()
      || interval_intensity <= 0
  ) {
    getNextInterval();
  }

  if (interval_time > 0 && interval_intensity > 0) {
    Car *car = new Car(current_id++, 0, config);
    last_car_time += gaussrand();

    interval_intensity--;

    car->setTimeIn(last_car_time);
    car->setExpectedTime(0);

    return car;
  } else {
    return NULL;
  }
}

void SeattleCarFactory::processHeader() {

  intensity_column = occupancy_column =  -1;
  travel_time_column = time_column = -1;
  string intensity("\"");
  intensity += config->getSTCFColumn();
  intensity += ".intensity\"";

  string occupancy("\"");
  occupancy += config->getSTCFColumn();
  occupancy += ".occupancy\"";

  std::vector<std::string> line = nextLine();

  for(int i = 0; i < 42; ++i) {
    if (line.at(i) == intensity) {
      intensity_column = i;
    } else
    if (line.at(i) == occupancy) {
      occupancy_column = i;
    } else
    if (line.at(i) == "\"TravelTime\"") {
      travel_time_column = i;
    } else
    if (line.at(i) == "\"Time\"") {
      time_column = i;
    }
  }

  if (intensity_column == -1) {
    cerr << intensity << " column not found.\n";
    exit(-1);
  }

  if (occupancy_column == -1) {
    cerr << occupancy << " column not found.\n";
    exit(-1);
  }

  if (time_column == -1) {
    cerr << "\"Time\" column not found.\n";
    exit(-1);
  }

  if (travel_time_column == -1) {
    cerr << "\"TravelTime\" column not found.\n";
    exit(-1);
  }
}
