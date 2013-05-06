#ifndef _CELL_H
#define _CELL_H

#include "car.h"

class Cell {

  // pozice bunky v ramci jedne cesty, pocitano od 0
  int position;

  Car * car;

  Cell * cell_front,
       * cell_back,
       * cell_left,
       * cell_right;

  public:

    Cell(int cell_position)
      : position(cell_position), car(NULL), cell_front(NULL), cell_back(NULL),
        cell_left(NULL), cell_right(NULL) {}

    Cell * getCellFront() { return cell_front; }
    Cell * getCellBack() { return cell_back; }
    Cell * getCellLeft() { return cell_left; }
    Cell * getCellRight() { return cell_right; }

    Car * getCar() { return car; }
    void setCar(Car * car) { this->car = car; }
    void free() { car = NULL; }

    bool isEmpty() { return car == NULL; }
    bool isOccupied() { return car != NULL; }

    void setCellFront(Cell * cell) { cell_front = cell; }
    void setCellBack(Cell * cell) { cell_back = cell; }
    void setCellLeft(Cell * cell) { cell_left = cell; }
    void setCellRight(Cell * cell) { cell_right = cell; }

    int getPosition() { return position; }
};

#endif
