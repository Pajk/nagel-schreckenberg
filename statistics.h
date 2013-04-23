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

  public:

    void logCarTime(int car_id, int total_time, int expected_time);
        
    float getMeanError();
    float getMeanErrorAbs();

    void print();
};

#endif

