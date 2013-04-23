#include <gtest/gtest.h>
#include "../config.h"

TEST (ConfigTest, LoadFromFile) { 
    Config config;
    config.loadFromFile("tests/nash.config");

    EXPECT_EQ (8, config.getSiteLength());
    EXPECT_EQ (5400, config.getTrackLength());
    EXPECT_EQ (1, config.getDefaultCar());

    // test default config
    CarConfig cc = config.getCarConfig(1232312);
    CarConfig default_cc = config.getDefaultCarConfig();
    EXPECT_EQ (default_cc, cc);

    // car config 0
    cc = config.getCarConfig(0);
    CarConfig test;
    test.slowdown_probability = 0.3;
    test.acceleration_probability = 1;
    test.max_speed = 5;
    test.min_speed = 1;
    EXPECT_EQ (test, cc);

    cc = config.getCarConfig(1);
    test.slowdown_probability = 0.5;
    test.acceleration_probability = 1;
    test.max_speed = 5;
    test.min_speed = 1;
    EXPECT_EQ (test, cc);

    cc = config.getCarConfig(2);
    test.slowdown_probability = 0.3;
    test.acceleration_probability = 1;
    test.max_speed = 4;
    test.min_speed = 1;
    EXPECT_EQ (test, cc);

    cc = config.getCarConfig(3);
    test.slowdown_probability = 0.5;
    test.acceleration_probability = 1;
    test.max_speed = 4;
    test.min_speed = 0;
    EXPECT_EQ (test, cc);

    cc = config.getCarConfig(4);
    test.slowdown_probability = 0.3;
    test.acceleration_probability = 1;
    test.max_speed = 3;
    test.min_speed = 1;
    EXPECT_EQ (test, cc);

    cc = config.getCarConfig(5);
    test.slowdown_probability = 0.55;
    test.acceleration_probability = 0.3;
    test.max_speed = 2;
    test.min_speed = 1;
    EXPECT_EQ (test, cc);
}

TEST (ConfigTest, LoadFromBinaryString) { 

    Config config;
    // format: site_length[4];slowdown_p[7];acc_p[7];max_sp[4];min_sp[4]
    // priklad pro 8;0.55;0.3;2;1
    // 1000 0110111 0011110 0010 0001
    config.loadFromBinaryString("10000110111001111000100001");

    EXPECT_EQ (8, config.getSiteLength());
    EXPECT_EQ (5400, config.getTrackLength());
    EXPECT_EQ (0, config.getDefaultCar());

    CarConfig cc = config.getCarConfig(0);
    EXPECT_EQ (cc, config.getCarConfig(1));

    CarConfig test;
    test.slowdown_probability = 0.55;
    test.acceleration_probability = 0.3;
    test.max_speed = 2;
    test.min_speed = 1;
    EXPECT_EQ (test, cc);
}

TEST (ConfigTest, LoadFromString) {
    Config config;
    config.loadFromBinaryString("01001100111111010110111001");
    config.print();
}

TEST (ConfigTest, LoadFromBinaryInteger) { 

    Config config;

    // format: site_length[4];slowdown_p[7];acc_p[7];max_sp[4];min_sp[4]
    // 26 bitu
    // priklad pro 8;0.55;0.3;2;1
    // 1000 0110111 0011110 0010 0001

    int binary_config = 0;
    const char *string_config = "10000110111001111000100001";

    for (int i = 25; i >=0; i--) {
        if (*string_config == '1') { 
            binary_config |= 1<<i;
        }
        string_config++;
    }
    config.loadFromInteger(binary_config);

    EXPECT_EQ (8, config.getSiteLength());
    EXPECT_EQ (5400, config.getTrackLength());
    EXPECT_EQ (0, config.getDefaultCar());

    CarConfig cc = config.getCarConfig(0);
    EXPECT_EQ (cc, config.getCarConfig(1));

    CarConfig test;
    test.slowdown_probability = 0.55;
    test.acceleration_probability = 0.3;
    test.max_speed = 2;
    test.min_speed = 1;
    EXPECT_EQ (test, cc);
}
