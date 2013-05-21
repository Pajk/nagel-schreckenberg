#include <cstdlib>

#include "simple_car_factory.h"
#include "../car.h"

Car *SimpleCarFactory::nextCar() {

    int car_class = rand() % car_types;

    Car *car = new Car(current_id++, car_class, config);

    last_car_time += config->getSCFInterval();
    car->setTimeIn(last_car_time);
    car->setExpectedTime(0);

    return car;
}
