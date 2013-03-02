#ifndef _car
#define _car

#include "track.h"

class Car {

  int id,
      max_speed,
      cur_speed,
      in_time,
      out_time,
      exp_time,
      car_class,
      position,
      old_position;

  float p;

  Car *car_in_front,
      *car_behind;

  Track *track;

  public:

    Car(int id, int car_class, Car *car_in_front, Track *track);
    ~Car();
 
    int getCurrentSpeed() { return cur_speed; }
    int getId() { return id; }
    int getMaximumSpeed() { return max_speed; }
    int getExpectedTime() { return exp_time; }
    int getCarClass() { return car_class; }
    Car * getCarInFront() { return car_in_front; }
    Car * getCarBehind() { return car_behind; }
    int getPosition() { return position; }
    
    void setCarInFront(Car *car) { car_in_front = car; }
    void setCarBehind(Car *car) { car_behind = car; }

    int getDistance() {
      return car_in_front ? car_in_front->getPosition() - position : 6;
    }

    void move();
};

#endif
