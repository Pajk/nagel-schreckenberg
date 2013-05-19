#include <cstdlib>
#include <cmath>
#include <iostream>

#include "normal_car_factory.h"
#include "../car.h"

double NormalCarFactory::gaussrand() {

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

Car *NormalCarFactory::nextCar() {

    int car_class = rand()%car_types;

    Car *car = new Car(current_id++, car_class, config);
    last_car_time += gaussrand();

    car->setTimeIn(last_car_time);
    car->setExpectedTime(0);

    return car;
}
