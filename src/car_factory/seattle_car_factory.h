#ifndef _SEATTLE_CAR_FACTORY_H
#define _SEATTLE_CAR_FACTORY_H

#include "csv_car_factory.h"
#include <iostream>

class SeattleCarFactory : public CsvCarFactory {

  int intensity_column;
  int occupancy_column;
  int time_column;
  int travel_time_column;

  int interval_time;
  float interval_travel_time;
  int interval_intensity;
  int interval_occupancy;

  long last_car_time;

  double V1, V2, S;
  int phase;
  float mean;
  float deviation;

  public:

    SeattleCarFactory(const char *file_name, Config *config) :
      CsvCarFactory(file_name, config) {

      column_separator = ';';

      fillBuffer();
      setConfig(config);
    }

    Car *nextCar();

    void processHeader();

    void getNextInterval();

    void setConfig(Config * config) {
      this->config = config;

      if (config != NULL) {
        intensity_column = 0;
        occupancy_column = 0;
        time_column = 0;
        travel_time_column = 0;

        interval_time = 0;
        interval_travel_time = 0;
        interval_intensity = 0;
        interval_occupancy = 0;
        last_car_time = 0;
        current_id = 1;
        phase = 0;

        resetIterator();
        processHeader();
      }
    }

  protected:

    double gaussrand();
};

#endif

