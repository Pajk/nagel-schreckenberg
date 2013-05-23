/*
 * Datum:   20.5.2013
 * Autor:   Pavel Pokorny, xpokor12@stud.fit.vutbr.cz
 * Projekt: DP - Kalibrace mikrosimulacniho modelu dopravy
 */

#ifndef _CELL_H
#define _CELL_H

#include "car.h"

class Cell {

  // pozice bunky v ramci jedne cesty, pocitano od 0
  int position;

  Car * car;

  Cell * cell_front,
       * cell_back;

  public:

    Cell(int cell_position) :
      position(cell_position),
      car(NULL),
      cell_front(NULL),
      cell_back(NULL) {}

    Cell * getCellFront() { return cell_front; }
    Cell * getCellBack() { return cell_back; }

    Car * getCar() { return car; }
    void setCar(Car * car) { this->car = car; }
    void free() { car = NULL; }

    bool isEmpty() { return car == NULL; }
    bool isOccupied() { return car != NULL; }

    void setCellFront(Cell * cell) { cell_front = cell; }
    void setCellBack(Cell * cell) { cell_back = cell; }

    int getPosition() { return position; }
};

#endif
