#include <iostream>
#include <cmath>
#include <iomanip>

#include "statistics.h"
#include "car.h"

using std::endl;
using std::cout;
using std::setw;
using std::setprecision;

Statistics::Statistics(bool table_format) {
    reset();
    this->table_format = table_format;
    if (table_format) {
        cout << " t_from,   t_to,     MAE,    MAPE,   flow, density, cars,  speed\n";
    }
}

void Statistics::reset(long time_from) {

    this->time_from = time_from;
    mae = 0;
    mape = 0;
    rmse = 0;
    flow = 0;
    density = 0;
    faster_mean_error = 0;
    slower_mean_error = 0;
    min_error = 0;
    max_error = 0;
    slower_cars = 0;
    faster_cars = 0;
    mean_travel_time = 0;
    cell_time_occupied = 0;
    mean_speed = 0;
    mean_expected_travel_time = 0;
    car_times.clear();
}

void Statistics::logCarTime(long current_time, Car * car) {

    int total_time = current_time - car->getTimeIn();
    CarTime time;
    time.car_id = car->getId();
    time.total_time = total_time;
    time.expected_time = car->getExpectedTime();
    time.left_at = current_time;
    car_times.push_back(time);
}

void Statistics::calculate(long time_to) {

    int slower_error_sum = 0,
        faster_error_sum = 0,
        abs_err = 0;

    min_error = 9999;
    max_error = -1;
    slower_cars = faster_cars = 0;

    mean_travel_time = mean_expected_travel_time = mae = rmse = flow = 0;
    CarTime ct;
    this->time_to = time_to;

    for (std::vector<CarTime>::iterator it = car_times.begin(); it != car_times.end(); ++it) {

        ct = *it;
        // cout << ct.car_id << " expected:" << ct.expected_time << " simulated:" << ct.total_time << " left_at:" << ct.left_at << endl;

        if (ct.expected_time > 0) {
            abs_err = abs(ct.total_time - ct.expected_time);

            if (min_error > abs_err) min_error = abs_err;
            if (max_error < abs_err) max_error = abs_err;

            if (ct.total_time > ct.expected_time) {
                slower_cars++;
                slower_error_sum += abs_err;
            } else {
                faster_cars++;
                faster_error_sum += abs_err;
            }

            mean_travel_time += ct.total_time;
            mean_expected_travel_time += ct.expected_time;

            mape += float(abs_err) / ct.expected_time;
            mae += abs_err;
            rmse += pow(abs_err, 2);
        }
    }
    float cars_size = car_times.size();

    // vypocet dopravni toku pro dany casovy interval
    long interval = time_to - time_from;
    flow = float(car_times.size()) / interval;

    // vypocet hustoty dopravy
    density = float(cell_time_occupied) / interval;

    // vypocet prumerne rychlosti
    mean_speed = mean_speed / float(interval);

    if (cars_size > 0) {
        mean_travel_time = mean_travel_time/cars_size;
        mean_expected_travel_time = mean_expected_travel_time/cars_size;
        mape = mape/cars_size*100;
        mae = mae/cars_size;
        rmse = sqrt(rmse/cars_size);
    } else {
        mae = 99999;
    }

    // cout << "Mean travel time: " << mean_travel_time << endl
    //     << "Expected mean travel time: " << mean_expected_travel_time << endl
    //     << "Summary times error: " << fabs(mean_travel_time - mean_expected_travel_time) << endl
    //     << "MAE: " << mae << endl
    //     << "MAPE: " << mape << endl
    //     << "RMSE: " << rmse << endl;

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

void Statistics::print() {

    if (car_times.size() == 0) {
        return;
    }

    if (table_format) {
        // "t_from t_to   MAE     MAPE    flow   density cars  speed"
        cout << std::fixed
             << setw(7) << time_from << ','
             << setw(7) << time_to << ','
             << setw(8) << setprecision(2) << mae << ','
             << setw(8) << setprecision(2) << mape << ','
             << setw(7) << setprecision(3) << flow << ','
             << setw(8) << setprecision(3) << density << ','
             << setw(5) << car_times.size() << ','
             << setw(7) << setprecision(2) << mean_speed << endl;

             // <<  << fixed << setprecision(2) << loop.price / 100.0
    } else {
        cout << car_times.size() << " cars" << endl
            << "mean absolute error: " << getMeanAbsoluteError() << "s" << endl
            << "mean absolute percentage error: " << getMeanAbsolutePercentageError() << "%" << endl
            << "traffic flow: " << getFlow() << endl
            << "traffic density: " << getDensity() << endl
            << "mean speed: " << getMeanSpeed() << endl
            << "number of cars: " << car_times.size() << endl
            << "min error: " << getMinError() << "s" << endl
            << "max error: " << getMaxError() << "s" << endl
            << "slower than expected: " << getSlowerCars()
            << ", mean error: " << getSlowerMeanError() << endl
            << "faster than expected: " << getFasterCars() << endl
            << ", mean error: " << getFasterMeanError() << endl;
    }
}

void Statistics::logCellOccupancy(bool occupied) {
    if (occupied) {
        cell_time_occupied++;
    }
}

void Statistics::logMeanSpeed(float mean_speed) {
    this->mean_speed += mean_speed;
}
