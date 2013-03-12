#include <cstdlib>

#include "simple_car_factory.h"
#include "car.h"

Car *SimpleCarFactory::nextCar() {

  int car_class = rand()%(config->getNumberOfCarTypes()-1);
  Car *car = new Car(current_id++, car_class, config);

  return car;
}
