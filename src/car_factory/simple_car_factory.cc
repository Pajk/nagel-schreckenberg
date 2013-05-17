#include <cstdlib>

#include "simple_car_factory.h"
#include "../car.h"

Car *SimpleCarFactory::nextCar() {

  int car_types = config->getNumberOfCarTypes();
  if (car_types > 1) car_types--;
  int car_class = rand()%car_types;
  Car *car = new Car(current_id++, car_class, config, statistics);

  car->setTimeIn(0);
  car->setExpectedTime(0);

  return car;
}
