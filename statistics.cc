#include <iostream>

#include "statistics.h"

void Statistics::logCarTime(int car_id, int total_time, int expected_time) {
    CarTime time;
    time.car_id = car_id;
    time.total_time = total_time;
    time.expected_time = expected_time;
    car_times.push_back(time);
}

float Statistics::getMeanError() {
    int error_sum = 0;
    int size = 0;
    for (std::vector<CarTime>::iterator it = car_times.begin(); it != car_times.end(); ++it) {
       
        if ((*it).expected_time > 0) {
            error_sum += (*it).total_time - (*it).expected_time;
            size++;
        }
    }
    if (size > 0) {
        return float(error_sum)/float(size);
    } else {
        return 99999;
    }
}

float Statistics::getMeanErrorAbs() {
    int error_sum = 0;
    int size = 0;
    for (std::vector<CarTime>::iterator it = car_times.begin(); it != car_times.end(); ++it) {
       
        if ((*it).expected_time > 0) {
            error_sum += abs((*it).total_time - (*it).expected_time);
            size++;
        }
    }
    if (size > 0) {
        return float(error_sum)/float(size);
    } else {
        return 99999;
    }
}
void Statistics::print() {
    std::cout << "mean error: " << getMeanError() << std::endl;
}
