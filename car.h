#ifndef _car
#define _car

#include "track.h"

class Car {

  int id,
      max_speed,
      current_speed,
      expected_time,
      car_class,
      position,
      old_position;

  long time_in;

  float p;

  Car *car_in_front,
      *car_behind;

  Track *track;

  public:

    Car(long id, int car_class);
    ~Car();
 
    int getCurrentSpeed() { return current_speed; }
    int getId() { return id; }
    int getMaximumSpeed() { return max_speed; }
    int getExpectedTime() { return expected_time; }
    int getCarClass() { return car_class; }
    Car * getCarInFront() { return car_in_front; }
    Car * getCarBehind() { return car_behind; }
    int getPosition() { return position; }
    long getTimeIn() { return time_in; }
    
    void setCarInFront(Car *car) { car_in_front = car; }
    void setCarBehind(Car *car) { car_behind = car; }
    void setTimeIn(long time) { time_in = time; }
    void setExpectedTime(int time) { expected_time = time; }

    int getDistance() {
      return car_in_front ? car_in_front->getPosition() - position : 6;
    }

    void start(Track *track, Car *car_in_front);
    void move();
};

#endif
