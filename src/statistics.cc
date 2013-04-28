#include <iostream>

#include "statistics.h"

void Statistics::logCarTime(int car_id, int total_time, int expected_time) {
    CarTime time;
    time.car_id = car_id;
    time.total_time = total_time;
    time.expected_time = expected_time;
    car_times.push_back(time);
}

void Statistics::count() {
    int error_sum = 0,
        slower_error_sum = 0,
        faster_error_sum = 0,
        tmp = 0;

    min_error = 9999;
    max_error = -1;
    slower_cars = 0;
    faster_cars = 0;

    for (std::vector<CarTime>::iterator it = car_times.begin(); it != car_times.end(); ++it) {

        if ((*it).expected_time > 0) {
            tmp = abs((*it).total_time - (*it).expected_time);
            if (min_error > tmp) min_error = tmp;
            if (max_error < tmp) max_error = tmp;
            error_sum += tmp;

            if ((*it).total_time > (*it).expected_time) {
                slower_cars++;
                slower_error_sum += tmp;
            } else {
                faster_cars++;
                faster_error_sum += tmp;
            }
        }
    }
    if (car_times.size() > 0) {
        mean_error = float(error_sum)/float(car_times.size());
    } else {
        mean_error = 99999;
    }

    if (slower_cars > 0) {
        slower_mean_error = float(slower_error_sum)/float(slower_cars);
    } else {
        slower_mean_error = 99999;
    }

    if (faster_cars > 0) {
        faster_mean_error = float(faster_error_sum)/float(faster_cars);
    } else {
        faster_mean_error = 99999;
    }
}

float Statistics::getMeanError() {
    if (mean_error == -1) count();
    return mean_error;
}

float Statistics::getSlowerMeanError() {
    if (slower_mean_error == -1) count();
    return slower_mean_error;
}

float Statistics::getFasterMeanError() {
    if (faster_mean_error == -1) count();
    return faster_mean_error;
}

int Statistics::getMinError() {
    if (min_error == -1) count();
    return min_error;
}

int Statistics::getMaxError() {
    if (max_error == -1) count();
    return max_error;
}

int Statistics::getSlowerCars() {
    if (slower_cars == -1) count();
    return slower_cars;
}

int Statistics::getFasterCars() {
    if (faster_cars == -1) count();
    return faster_cars;
}

void Statistics::print() {
    std::cout << car_times.size() << " cars" << std::endl
              << "mean error: " << getMeanError() << "s" << std::endl
              << "min error: " << getMinError() << "s" << std::endl
              << "max error: " << getMaxError() << "s" << std::endl
              << "slower than expected: " << getSlowerCars()
              << ", mean error: " << getSlowerMeanError() << std::endl
              << "faster than expected: " << getFasterCars()
              << ", mean error: " << getFasterMeanError() << std::endl;
}
