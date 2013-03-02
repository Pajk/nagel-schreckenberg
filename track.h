#ifndef _track
#define _track

class Car;

class Track {

  int length,
      number_of_cars,
      sim_time;

  Car *left_car,
      *car;

  public:
    Track(int length);
    ~Track();
 
    int getCurrentTime() { return sim_time; }
    int getLength() { return length; }
    Car * getLeftCar() { return left_car; }
    void setLeftCar(Car *car) { left_car = car; }

    void step();
};

#endif
