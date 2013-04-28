#ifndef _STATISTICS_H
#define _STATISTICS_H

#include <vector>
#include <cstdlib>

struct CarTime {
  int total_time;
  int expected_time;
  int car_id;
};

class Statistics {

  std::vector<CarTime> car_times;

  float mean_error;
  float faster_mean_error;
  float slower_mean_error;
  int min_error;
  int max_error;
  int slower_cars;
  int faster_cars;

  public:

    Statistics() :
      mean_error(-1),
      min_error(-1),
      max_error(-1),
      slower_cars(-1),
      faster_cars(-1),
      slower_mean_error(-1),
      faster_mean_error(-1) {}

    void logCarTime(int car_id, int total_time, int expected_time);

    void count();
    float getMeanError();
    float getSlowerMeanError();
    float getFasterMeanError();
    int getMaxError();
    int getMinError();
    int getSlowerCars();
    int getFasterCars();

    void print();
};

#endif

