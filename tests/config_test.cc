#include <gtest/gtest.h>
#include "../src/config.h"

TEST (ConfigTest, LoadFromFile) {
    Config config;
    config.loadFromFile("tests/nash.config");

    EXPECT_EQ (2.5, config.getSiteLength());
    EXPECT_EQ (5400, config.getTrackLength());
    EXPECT_EQ (6, config.getTrackMaxSpeed());
    EXPECT_EQ (1, config.getDefaultCar());
    EXPECT_EQ (2160, config.getNumberOfTrackCells());

    // test default config
    CarConfig cc = config.getCarConfig(1232312);
    CarConfig default_cc = config.getDefaultCarConfig();
    EXPECT_EQ (default_cc, cc);

    // car config 0
    cc = config.getCarConfig(0);
    EXPECT_EQ (0, cc.car_class);
    CarConfig test;
    test.slowdown_probability = 0.3;
    test.acceleration_probability = 1;
    test.max_speed = 5;
    test.min_speed = 1;
    test.length = 2;
    EXPECT_EQ (test, cc);

    cc = config.getCarConfig(1);
    EXPECT_EQ (1, cc.car_class);
    test.slowdown_probability = 0.5;
    test.acceleration_probability = 1;
    test.max_speed = 5;
    test.min_speed = 1;
    test.length = 3;
    EXPECT_EQ (test, cc);

    cc = config.getCarConfig(2);
    EXPECT_EQ (2, cc.car_class);
    test.slowdown_probability = 0.3;
    test.acceleration_probability = 1;
    test.max_speed = 4;
    test.min_speed = 1;
    test.length = 5;
    EXPECT_EQ (test, cc);

    cc = config.getCarConfig(3);
    EXPECT_EQ (3, cc.car_class);
    test.slowdown_probability = 0.5;
    test.acceleration_probability = 1;
    test.max_speed = 4;
    test.min_speed = 0;
    test.length = 6;
    EXPECT_EQ (test, cc);

    cc = config.getCarConfig(4);
    EXPECT_EQ (4, cc.car_class);
    test.slowdown_probability = 0.3;
    test.acceleration_probability = 1;
    test.max_speed = 3;
    test.min_speed = 1;
    test.length = 7;
    EXPECT_EQ (test, cc);

    cc = config.getCarConfig(5);
    EXPECT_EQ (5, cc.car_class);
    test.slowdown_probability = 0.55;
    test.acceleration_probability = 0.3;
    test.max_speed = 2;
    test.min_speed = 1;

    test.length = 8;
    EXPECT_EQ (test, cc);
}

TEST (ConfigTest, LoadFromInteger) {

    Config config;

    // format: car_length[3]; max_speed[4]; slowdown_probability[7]; acceleration_probability[7];
    // 21 bitu
    // BITY   HODNOTA                     POCET BITU
    // 20-18  car_length                  3
    // 17-14  max_speed                   4
    // 13-7   slowdown_probability        7
    // 6-0    acceleration_probability    7
    // Delka vozidla <1, 8>
    // Maximalni rychlost vozidla <1, 16>
    // Pravdepodobnost zpomaleni <0, 1>
    // Pravdepodobnost zrychleni <0, 1>

    // priklad pro 3;6;0.5;0.5
    // 010 0101 0111111 0111111
    // 2   5    63      63

    int integer_config = 0;
    const char *string_config = "010010101111110111111";

    // zakodovani do integeru
    for (int i = strlen(string_config) - 1; i >=0; i--) {
        if (*string_config == '1') {
            integer_config |= 1<<i;
        }
        string_config++;
    }
    config.loadFromInteger(integer_config);

    EXPECT_EQ (2.5, config.getSiteLength());
    EXPECT_EQ (5350, config.getTrackLength());
    EXPECT_EQ (0, config.getDefaultCar());

    CarConfig cc = config.getCarConfig(0);
    EXPECT_EQ (cc, config.getCarConfig(1));

    CarConfig test;
    test.slowdown_probability = 0.5;
    test.acceleration_probability = 0.5;
    test.max_speed = 6;
    test.min_speed = 1;
    test.length = 3;
    test.car_class = 0;
    // test.print();
    // cc.print();
    EXPECT_EQ (test, cc);
}
