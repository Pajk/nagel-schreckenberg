#ifndef _CAR_H
#define _CAR_H

#include "config.h"

class Cell;
class Track;

class Car {

  int id,
      max_speed,
      min_speed,
      current_speed,
      expected_time,
      car_class,
      position,
      old_position,
      length;

  // poznaceni, ze auto v minulek kroku stalo z duvodu uplatneni slow-to-start
  bool slow_start;

  unsigned long long time_in;

  float slowdown_probability,
        acceleration_probability;

  Car *car_in_front,
      *car_behind;

  Track *track;

  Cell * cell;

  Config *config;

  public:

    Car(long id, int car_class, Config *config);
    ~Car();

    int getCurrentSpeed() { return current_speed; }
    int getId() { return id; }
    int getExpectedTime() { return expected_time; }
    int getCarClass() { return car_class; }
    Car * getCarInFront() { return car_in_front; }
    Car * getCarBehind() { return car_behind; }
    int getPosition() { return position; }
    int getBackPosition() { return position - length; }
    unsigned long long getTimeIn() { return time_in; }
    long getExpectedTimeOut() { return time_in + expected_time; }
    int getLength() { return length; }
    Track * getTrack() { return track; }

    void setCarInFront(Car *car) { car_in_front = car; }
    void setCarBehind(Car *car) { car_behind = car; }
    void setTimeIn(long time) { time_in = time; }
    void setExpectedTime(int time) { expected_time = time; }
    void setCell(Cell * cell) { this->cell = cell; }

    /**
     * Vozidlo vjede na zadanou vozovku
     */
    void enterTrack(Track *track);

    /**
     * Vozidlo vykona jeden krok simulace, dojde k vypocitani nove rychlosti
     * a posunu vozidla na novou pozici
     */
    void step();
    void calculateSpeed();
    void move();

  protected:

    void advanceCells(int cells);

    void loadCarConfig();

    void basicStep();

    void slowToStopStep();

    /**
     * Vrati pocet volnych bunek pred autem
     * @param visibility Kolik predchazejicich bunek se ma maximalne zkontrolovat
     */
    int getFreeCellsCount(int visiblity);

    int getCarAheadSpeed(int free_cells);
};

#endif
