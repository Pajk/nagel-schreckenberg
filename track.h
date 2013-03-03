#ifndef _track
#define _track

class Car;
class CarFactory;
struct tm;

class Track {

  int length,
      number_of_cars,
      sim_time;

  Car *last_car,
      *next_car,
      *car;

  long time_offset;

  struct tm *start_time;

  CarFactory *car_factory;

  public:
    Track(CarFactory *car_factory, int length);
    ~Track();
 
    int getCurrentTime() { return sim_time; }
    int getLength() { return length; }
    Car * getLastCar() { return last_car; }
    void setLastCar(Car *car) { last_car = car; }

    void step();
    bool live();
};

#endif
