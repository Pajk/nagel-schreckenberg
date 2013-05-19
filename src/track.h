#ifndef _track
#define _track

class Car;
class CarFactory;
class Cell;
class Config;
class World;
struct tm;

class Track {

  int length;

  World * world;

  Car * last_car,
      * next_car;

  Cell * first_cell,
       * last_cell,
       * middle_cell;

  CarFactory *car_factory;

  long time_offset;

  bool periodic_boundary;

  // realny cas prijezdu prvniho vozidla na vozovku ziskany z empirickych dat,
  // pokud se jedna o simulaci na zaklade dat, ktere tento udaj obsahuji,
  // pomoci tohoto casu je mozne prevadet cas simulace na realny cas
  // struct tm *start_time;

  public:

    Track(Config * config, CarFactory * car_factory, World * world = NULL);
    ~Track();

    /**
     * Vrati delku vozovky v poctu bunek
     */
    int getLength() { return length; }

    /**
     * Vrati referenci na posledni auto, ktere je na vozovce
     */
    Car * getLastCar() { return last_car; }

    /**
     * Nastaveni reference na posledni auto, ktere je na vozovce
     */
    void setLastCar(Car * car) { last_car = car; }

    /**
     * Vrati referenci na prvni bunky vozovky
     */
    Cell * getFirstCell() { return first_cell; }

    /**
     * Vrati referenci na posledni bunku vozovky
     */
    Cell * getLastCell() { return last_cell; }

    /**
     * Vrati referenci na bunku uprostred vozovky
     */
    Cell * getMiddleCell() { return middle_cell; }

    /**
     * Vrati referenci na objekt ridici simulaci
     */
    World * getWorld() { return world; }

    /**
     * Nastavi referenci na ridici objekt
     */
    void setWorld(World * world) { this->world = world; }

    /**
     * Vraci true pokud jsou pouzity periodicke hranicni podminky
     */
    bool hasPeriodicBoundary() { return periodic_boundary; }

    /**
     * Vykona jeden krok simulace, posune vozidla na nove pozice.
     */
    void step();

    /**
     * Predikat, ktery znaci, ze jsou na vozovce jeste nejaka vozidla nebo
     * nejake vozidlo ceka na vhodny cas.
     */
    bool isLive();

  protected:

    /**
     * Vyvoreni objektu bunek ze kterych se tato cesta sklada
     */
    void createCells();

    /**
     * Slouzi ke zjisteni, zda danemu autu je povolen vjezd na vozovku.
     * Aktualni cas simulace musi byt vetsi nez naplanovany cas vjezdu vozidla
     * a na zacatku vozovky musi byt dostatek volnych bunek pro dane auto.
     */
    bool isEnterAllowed(Car * car);
};

#endif
