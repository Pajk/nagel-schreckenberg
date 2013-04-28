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

  long time_in;

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
    int getMaximumSpeed() { return max_speed; }
    int getExpectedTime() { return expected_time; }
    int getCarClass() { return car_class; }
    Car * getCarInFront() { return car_in_front; }
    Car * getCarBehind() { return car_behind; }
    int getPosition() { return position; }
    int getBackPosition() { return position - length; }
    long getTimeIn() { return time_in; }
    int getLength() { return length; }
    Cell * getOccupiedCell() { return cell; }

    void setCarInFront(Car *car) { car_in_front = car; }
    void setCarBehind(Car *car) { car_behind = car; }
    void setTimeIn(long time) { time_in = time; }
    void setExpectedTime(int time) { expected_time = time; }

    int getDistance() {
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

    void start(Track *track, Car *car_in_front, Cell *first_cell);
    void move();
    void loadCarConfig();
    void advanceCells(int cells);
};

#endif
