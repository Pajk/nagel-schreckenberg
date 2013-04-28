#include <gtest/gtest.h>
#include "../src/config.h"
#include "../src/car_factory/dummy_car_factory.h"
#include "../src/track.h"
#include "../src/cell.h"

TEST (TrackTest, createCells) {

    CarFactory *car_factory = new DummyCarFactory();
    Track track(car_factory, 876);

    EXPECT_EQ (876, track.getLength());

    Cell * cell = track.getFirstCell(), * tmp_cell;

    int i = 0;
    while (cell) {
        i++;
        tmp_cell = cell;
        cell = cell->getCellFront();
    }

    EXPECT_EQ (876, i);
    EXPECT_EQ (tmp_cell,  track.getLastCell());

    cell = track.getFirstCell();
    for (int i = 0; i < 876; ++i) {
        EXPECT_EQ (i, cell->getPosition());
        cell = cell->getCellFront();
    }
    EXPECT_EQ (NULL, cell);

    delete car_factory;
}
