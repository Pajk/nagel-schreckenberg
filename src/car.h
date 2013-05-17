#ifndef _CAR_H
#define _CAR_H

#include "track.h"
#include "config.h"
#include "statistics.h"

class Cell;

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

  unsigned long long time_in;

  float slowdown_probability,
        acceleration_probability;

  Car *car_in_front,
      *car_behind;

  Track *track;

  Cell * cell;

  Config *config;

  Statistics *statistics;

  public:

    Car(long id, int car_class, Config *config, Statistics *statistics);
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

    void setCarInFront(Car *car) { car_in_front = car; }
    void setCarBehind(Car *car) { car_behind = car; }
    void setTimeIn(long time) { time_in = time; }
    void setExpectedTime(int time) { expected_time = time; }

    /**
     * Vozidlo vjede na zadanou vozovku
     */
    void enterTrack(Track *track);

    /**
     * Vozidlo vykona jeden krok simulace, dojde k vypocitani nove rychlosti
     * a posunu vozidla na novou pozici
     */
    void step();

  protected:

    void advanceCells(int cells);

    void loadCarConfig();

    /**
     * Vrati pocet volnych bunek pred autem
     */
    int getFreeCellsCount() {
      if (car_in_front)
        // pred autem jede jine auto, vrati se vzdalenost od posledni
        // bunky, kterou auto zabira
        return car_in_front->getBackPosition() - position;
      else
        // auto je prvni na silnici, pred nim uz zadne dalsi nejede
        // vrati se vzdalenost od konce vozovky + neco navic, aby bylo
        // auto schopne vyjet z vozovky
        return track->getLength() - position + 10;
    }
};

#endif
