#include <cstdlib>

#include "simple_car_factory.h"
#include "car.h"

Car *SimpleCarFactory::nextCar() {

  Car *car = new Car(current_id++, rand()%3);

  return car;
}
