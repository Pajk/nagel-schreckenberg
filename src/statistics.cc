#include <iostream>
#include <cmath>
#include <iomanip>
#include <cstring>

#include "statistics.h"
#include "car.h"
#include "config.h"

using std::endl;
using std::cout;
using std::setw;
using std::setprecision;
using std::vector;

Statistics::Statistics(Config * config, bool suppress_output) {
    this->config = config;

    table_format = config->getTableFormat();

    reset();
    summaryReset();
    this->suppress_output = suppress_output;
    if (table_format && !suppress_output) printHeader();
}

void Statistics::reset(long time_from) {

    this->time_from = time_from;
    cell_time_occupied = 0;
    mean_speed = 0;
    car_times.clear();
    memset(&interval_data, 0, sizeof(interval_data));
}

void Statistics::logCarTime(long current_time, Car * car) {

    int total_time = current_time - car->getTimeIn();
    // cout << "car " << car->getId() << " ends after " << total_time << " " << current_time << " " << car->getTimeIn() << endl;
    CarTime ct;
    ct.car_id = car->getId();
    ct.total_time = total_time;
    ct.expected_time = car->getExpectedTime();
    ct.left_at = current_time;
    car_times.push_back(ct);
}

void Statistics::calculate(long time_to) {

    int abs_err = 0;

    interval_data.min_error = 9999;
    interval_data.max_error = -1;
    interval_data.t_to = time_to;
    interval_data.t_from = time_from;

    for (vector<CarTime>::iterator it = car_times.begin(); it != car_times.end(); ++it) {

        CarTime &ct = *it;
        // cout << ct.car_id << " expected:" << ct.expected_time << " simulated:" << ct.total_time << " left_at:" << ct.left_at << endl;

        // pokud je k dispozici ocekavany cas dojezdu pro konkretni vozidla
        // vypocita se chyba simulace

        if (ct.expected_time > 0) {
            abs_err = abs(ct.total_time - ct.expected_time);
            if (interval_data.min_error > abs_err) interval_data.min_error = abs_err;
            if (interval_data.max_error < abs_err) interval_data.max_error = abs_err;
            if (ct.total_time > ct.expected_time) {
                interval_data.slower_cars++;
                interval_data.slower_mae += abs_err;
            } else {
                interval_data.faster_cars++;
                interval_data.faster_mae += abs_err;
            }

            interval_data.mean_travel_time += ct.total_time;
            interval_data.mean_expected_travel_time += ct.expected_time;

            interval_data.mae += abs_err;
            interval_data.mape += float(abs_err) / ct.expected_time;
            interval_data.rmse += pow(abs_err, 2);
        }
    }
    interval_data.cars = car_times.size();

    // vypocet prumerne chyby
    if (interval_data.cars > 0) {
        float cars = interval_data.cars;
        interval_data.mean_travel_time /= cars;
        interval_data.mean_expected_travel_time /= cars;
        interval_data.mape /= cars;
        interval_data.mape *= 100.0;
        interval_data.mae /= cars;
        interval_data.rmse = sqrt(interval_data.rmse/cars);

        // vypocet dopravni toku pro dany casovy interval
        long interval = interval_data.t_to - interval_data.t_from;
        interval_data.flow = float(interval_data.cars) / (interval / 3600.0);

        // vypocet prumerne rychlosti v jednotkach pocet bunek za sekundu
        interval_data.mean_speed = (config->getTrackLength() / 1000.0)
            / (interval_data.mean_travel_time / 3600);

        // vypocet hustoty dopravy
        if (interval_data.mean_speed > 0) {
            interval_data.density =  interval_data.flow / interval_data.mean_speed;
        } else {
            interval_data.density = 0;
        }
    }

    if (interval_data.slower_cars > 0) {
        interval_data.slower_mae /= interval_data.slower_cars;
    }

    if (interval_data.faster_cars > 0) {
        interval_data.faster_mae /= interval_data.faster_cars;
    }

    // ulozeni vypocitanych dat do vektoru historie
    history.push_back(interval_data);
}

void Statistics::print() {

    if (suppress_output) {
        return;
    }

    if (table_format) {
        // "t_from t_to   MAE     MAPE    flow   density cars  speed"
        cout << std::fixed
             << setw(7) << interval_data.t_from << ','
             << setw(7) << interval_data.t_to << ','
             << setw(8) << setprecision(2) << interval_data.mae << ','
             << setw(8) << setprecision(2) << interval_data.mape << ','
             << setw(7) << setprecision(0) << interval_data.flow << ','
             << setw(8) << setprecision(3) << interval_data.density << ','
             << setw(5) << interval_data.cars << ','
             << setw(7) << setprecision(2) << interval_data.mean_speed << ','
             << setw(7) << setprecision(2) << interval_data.mean_travel_time << endl;

             // <<  << fixed << setprecision(2) << loop.price / 100.0
    } else {
        cout << "\n====================================\n"
            << interval_data.cars << " cars" << endl
            << "mean absolute error: " << interval_data.mae << "s" << endl
            << "mean absolute percentage error: " << interval_data.mape << "%" << endl
            << "traffic flow: " << interval_data.flow << endl
            << "traffic density: " << interval_data.density << endl
            << "mean speed: " << interval_data.mean_speed << endl
            << "number of cars: " << interval_data.cars << endl
            << "min error: " << interval_data.min_error << "s" << endl
            << "max error: " << interval_data.max_error << "s" << endl
            << "slower than expected: " << interval_data.slower_cars
            << ", mean error: " << interval_data.slower_mae << endl
            << "faster than expected: " << interval_data.faster_cars
            << ", mean error: " << interval_data.faster_mae << endl;
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

void Statistics::summaryCalculate() {

    vector<Summary>::iterator it = history.begin();

    summary_data.t_from = (*it).t_from;
    summary_data.min_error = (*it).min_error;
    summary_data.max_error = (*it).max_error;

    for (; it != history.end(); ++it) {

        summary_data.mae += (*it).mae;
        summary_data.mape += (*it).mape;
        summary_data.rmse += (*it).rmse;
        summary_data.flow += (*it).flow;
        summary_data.density += (*it).density;
        summary_data.mean_speed += (*it).mean_speed;
        summary_data.cars += (*it).cars;
        summary_data.faster_cars += (*it).faster_cars;
        summary_data.slower_cars += (*it).slower_cars;
        summary_data.faster_mae += (*it).faster_mae;
        summary_data.slower_mae += (*it).slower_mae;
        summary_data.mean_travel_time += (*it).mean_travel_time;
        summary_data.mean_expected_travel_time += (*it).mean_expected_travel_time;
    }

    --it;
    summary_data.t_to = (*it).t_to;

    int size = history.size();

    summary_data.mae /= size;
    summary_data.mape /= size;
    summary_data.rmse /= size;
    summary_data.flow /= size;
    summary_data.density /= size;
    summary_data.mean_speed /= size;
    summary_data.cars /= size;
    summary_data.faster_cars /= size;
    summary_data.slower_cars /= size;
    summary_data.faster_mae /= size;
    summary_data.slower_mae /= size;
    summary_data.mean_travel_time /= size;
    summary_data.mean_expected_travel_time /= size;
}

void Statistics::summaryPrint() {

    struct Summary tmp = interval_data;
    bool stmp = suppress_output;
    interval_data = summary_data;
    if (!suppress_output) {
        cout << "================================================================\n";
    }
    suppress_output = false;
    print();
    interval_data = tmp;
    suppress_output = stmp;
}

void Statistics::summaryReset() {

    memset(&summary_data, 0, sizeof(summary_data));
    history.clear();
}

void Statistics::printHeader() {
    cout << " t_from,   t_to,     MAE,    MAPE,   flow, density, cars,  speed,   time\n";
}
