#include <iostream>
#include <cmath>

#include "statistics.h"

using std::endl;
using std::cout;

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
        abs_err = 0;

    min_error = 9999;
    max_error = -1;
    slower_cars = 0;
    faster_cars = 0;

    mean_travel_time = 0;
    mean_expected_travel_time = 0;
    float mape = 0;
    float mae = 0;
    float rmse = 0;


    for (std::vector<CarTime>::iterator it = car_times.begin(); it != car_times.end(); ++it) {

        if ((*it).expected_time > 0) {
            abs_err = abs((*it).total_time - (*it).expected_time);

            if (min_error > abs_err) min_error = abs_err;
            if (max_error < abs_err) max_error = abs_err;
            error_sum += abs_err;

            if ((*it).total_time > (*it).expected_time) {
                slower_cars++;
                slower_error_sum += abs_err;
            } else {
                faster_cars++;
                faster_error_sum += abs_err;
            }

            mean_travel_time += (*it).total_time;
            mean_expected_travel_time += (*it).expected_time;

            mape += float(abs_err) / (*it).expected_time;
            mae += abs_err;
            rmse += pow(abs_err, 2);
        }
    }
    float cars_size = car_times.size();
    if (cars_size > 0) {
        mean_error = float(error_sum)/cars_size;
        mean_travel_time = mean_travel_time/cars_size;
        mean_expected_travel_time = mean_expected_travel_time/cars_size;
        mape = mape/cars_size*100;
        mae = mae/cars_size;
        rmse = sqrt(rmse/cars_size);
    } else {
        mean_error = 99999;
    }

    cout << "Mean travel time: " << mean_travel_time << endl
        << "Expected mean travel time: " << mean_expected_travel_time << endl
        << "Summary times error: " << fabs(mean_travel_time - mean_expected_travel_time) << endl
        << "MAE: " << mae << endl
        << "MAPE: " << mape << endl
        << "RMSE: " << rmse << endl;

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

float Statistics::getMeanTravelTime() {
    if (mean_travel_time == -1) count();
    return mean_travel_time;
}

float Statistics::getMeanExpectedTravelTime() {
    if (mean_expected_travel_time == -1) count();
    return mean_expected_travel_time;
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
