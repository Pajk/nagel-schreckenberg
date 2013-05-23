#include <gtest/gtest.h>
#include "../src/car_factory/seattle_car_factory.h"
#include "../src/car.h"
#include <iostream>

using std::cout;
using std::endl;

TEST (SeattleCarFactoryTest, nextCar) {
    srand(time(0));
    Config config;
    config.setStatsFrequency(900);
    config.setSTCFColumn("X1230");
    CarFactory * car_factory = new SeattleCarFactory(
        "tests/seattle_test.txt",
        &config
    );

    int cars = 0;
    int offset = 0;
    for (int i = 0; i < 900; ++i) {
        Car * car = car_factory->nextCar();
        if (offset == 0) offset = car->getTimeIn();
        if (car != NULL) {
            // cout << (car->getTimeIn() - offset)/60.0 << endl;
            delete car;
            cars++;
        } else {
            break;
        }
    }

    EXPECT_LT(51+48+36-10, cars);
    EXPECT_GT(51+48+36+10, cars);
}
