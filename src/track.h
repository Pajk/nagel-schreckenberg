#ifndef _track
#define _track

class Car;
class CarFactory;
class Cell;
struct tm;

class Track {

  int length,
      number_of_cars;

  unsigned long long sim_time;

  Car * last_car,
      * next_car,    // auto, ktere ma vjet na cestu jako dalsi v poradi
      * car;

  Car * delete_list; // auta, ktera vyjela z vozovky

  Cell * first_cell,
       * last_cell;

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

    Cell * getFirstCell() { return first_cell; }
    Cell * getLastCell() { return last_cell; }

    /**
     * Vykona jeden krok simulace, posune auta na nove pozice.
     */
    void step();

    /**
     * Kontrola, jestli jsou na ceste jeste nejaka auta nebo jestli
     * existuje jeste auto, ktere na cestu teprve vjede.
     */
    bool isLive();

    void addOutOfTrackCar(Car * car);

  protected:
    void createCells();
    bool isEnterAllowed(Car * car);
};

#endif
