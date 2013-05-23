/*
 * Datum:   20.5.2013
 * Autor:   Pavel Pokorny, xpokor12@stud.fit.vutbr.cz
 * Projekt: DP - Kalibrace mikrosimulacniho modelu dopravy
 */

#include <cmath>
#include <bitset>
#include <gtest/gtest.h>
#include <ga/GA1DBinStrGenome.h>

#include "../src/config.h"

using namespace std;

#define KMPH_TO_CELL_PER_SEC(X) roundf(float(X)/3.6/float(site_length))
#define M_TO_CELL(X) roundf(X/site_length)

TEST (ConfigTest, LoadFromFile) {
    Config config;
    config.loadFromFile("tests/nash.config");

    EXPECT_EQ (2.5, config.getSiteLength());
    EXPECT_EQ (5400, config.getTrackLength());
    EXPECT_EQ (1, config.getDefaultCar());
    EXPECT_EQ (2160, config.getNumberOfTrackCells());

    // test default config
    CarConfig cc = config.getCarConfig(1232312);
    CarConfig default_cc = config.getDefaultCarConfig();
    EXPECT_EQ (default_cc, cc);

    // car config 0
    // v testovacim config souboru je nastavena delka jedne bunky na 2.5 m,
    // max rychlost na 50 km/h, delka vozidla na 5 m, v nastaveni jsou ulozeny
    // jiz prepocitane hodnoty v bunkach za sekundu
    cc = config.getCarConfig(0);
    EXPECT_EQ (0, cc.car_class);
    CarConfig test;
    test.slowdown_probability = 0.3;
    test.acceleration_probability = 1;
    test.max_speed = 6;
    test.min_speed = 1;
    test.length = 2;
    EXPECT_EQ (test, cc);
    // test.print();
    // cc.print();

    float site_length = config.getSiteLength();

    cc = config.getCarConfig(1);
    EXPECT_EQ (1, cc.car_class);
    test.slowdown_probability = 0.5;
    test.acceleration_probability = 1;
    test.max_speed = KMPH_TO_CELL_PER_SEC(50);
    test.min_speed = KMPH_TO_CELL_PER_SEC(10);
    test.length = M_TO_CELL(7.5);
    EXPECT_EQ (test, cc);
    // test.print();
    // cc.print();

    cc = config.getCarConfig(2);
    EXPECT_EQ (2, cc.car_class);
    test.slowdown_probability = 0.3;
    test.acceleration_probability = 0.6;
    test.max_speed = KMPH_TO_CELL_PER_SEC(40);
    test.min_speed = KMPH_TO_CELL_PER_SEC(10);
    test.length = M_TO_CELL(10);
    EXPECT_EQ (test, cc);

    cc = config.getCarConfig(3);
    EXPECT_EQ (3, cc.car_class);
    test.slowdown_probability = 0.5;
    test.acceleration_probability = 1;
    test.max_speed = KMPH_TO_CELL_PER_SEC(40);
    test.min_speed = 0;
    test.length = M_TO_CELL(15);
    EXPECT_EQ (test, cc);

    cc = config.getCarConfig(4);
    EXPECT_EQ (4, cc.car_class);
    test.slowdown_probability = 0.6;
    test.acceleration_probability = 0.2;
    test.max_speed = KMPH_TO_CELL_PER_SEC(50);
    test.min_speed = KMPH_TO_CELL_PER_SEC(5);
    test.length = M_TO_CELL(15);
    EXPECT_EQ (test, cc);

    cc = config.getCarConfig(5);
    EXPECT_EQ (5, cc.car_class);
    test.slowdown_probability = 0.55;
    test.acceleration_probability = 0.3;
    test.max_speed = KMPH_TO_CELL_PER_SEC(40);
    test.min_speed = 0;
    test.length = M_TO_CELL(20);
    EXPECT_EQ (test, cc);

    EXPECT_EQ (5*60, config.getStatsFrequency());
    EXPECT_EQ (true, config.getTableFormat());
    EXPECT_EQ (1, config.getCarFactory());
    EXPECT_EQ (true, config.getPeriodicBoundary());
}

TEST (ConfigTest, LoadFromInteger) {

    Config config;

    // format: cell_length[4]; car_length[4]; max_speed[4]; slowdown_probability[7]; acceleration_probability[7];
    // 26 bitu
    // BITY   HODNOTA                     POCET BITU   POPIS + INTERVAL
    // 25-22  cell_length                 4            Delka bunky <1, 10>
    // 21-18  car_length                  4            Delka vozidla <5, 20>
    // 17-14  max_speed                   4            Maximalni rychlost vozidla <20, 60>
    // 13-7   slowdown_probability        7            Pravdepodobnost zpomaleni <0, 1>
    // 6-0    acceleration_probability    7            Pravdepodobnost zrychleni <0, 1>

    // priklad pro nastaveni:
    // 10m  20m  60km/h 0.5     0.5
    // 1111 1111 1111   0111111 0111111
    // 15   15   15      63      63

    // zakodovani do integeru
    bitset<32> string_config(string("11111111111101111110111111"));
    unsigned long ul = string_config.to_ulong();
    config.loadFromInteger(ul);

    int site_length = 10;
    EXPECT_EQ (site_length, config.getSiteLength());
    EXPECT_EQ (5350, config.getTrackLength());
    EXPECT_EQ (0, config.getDefaultCar());
    EXPECT_EQ (535, config.getNumberOfTrackCells());

    CarConfig cc = config.getCarConfig(0);
    EXPECT_EQ (cc, config.getCarConfig(1));

    CarConfig test;
    test.slowdown_probability = 0.5;
    test.acceleration_probability = 0.5;
    test.max_speed = KMPH_TO_CELL_PER_SEC(60);
    test.min_speed = 0;
    test.length = 2;
    test.car_class = 0;
    // test.print();
    // cc.print();
    EXPECT_EQ (test, cc);
}

TEST (ConfigTest, LoadFromGABinaryStringMaxValues) {

     int car_types = 6;
     int default_car = 1;
     int genome_length = BITS_TRACK + car_types * BITS_CAR;

    // HODNOTA                     POCET BITU           POPIS + INTERVAL

    // NASTAVENI TRATE             celkem BITS_TRACK
    // cell_length                 BITS_CELL_LENGTH     Delka bunky <1, 10>

    // NASTAVENI AUTA              celkem BITS_CAR
    // car_length                  BITS_CAR_LENGTH            Delka vozidla <5, 20>
    // max_speed                   BITS_MAX_SPEED            Maximalni rychlost vozidla <20, 60>
    // min_speed                   BITS_MIN_SPEED            Minimalni rychlost vozidla <0, 20>
    // slowdown_probability        BITS_SLOWDOWN_P            Pravdepodobnost zpomaleni <0, 1>
    // acceleration_probability    BITS_ACC_P            Pravdepodobnost zrychleni <0, 1>

    GA1DBinaryStringGenome genome(genome_length);

    // TEST KRAJNICH HODNOT NASTAVENI
    // nastaveni vozidel
    for (int i = 0; i < genome_length; ++i) {
        genome.gene(i, 1);
    }

    Config config;
    config.loadFromGABinaryString(genome, default_car);
    int track_length = 5000;
    float site_length = CELL_LENGTH_R;
    config.setTrackLength(track_length);

    EXPECT_EQ (int(site_length), int(config.getSiteLength()));
    EXPECT_EQ (track_length, config.getTrackLength());
    EXPECT_EQ (default_car, config.getDefaultCar());
    EXPECT_EQ (int(track_length/site_length), config.getNumberOfTrackCells());
    EXPECT_EQ (car_types, config.getNumberOfCarTypes());

    CarConfig cc = config.getCarConfig(default_car);
    EXPECT_EQ (cc, config.getCarConfig(-1));

    CarConfig test;
    test.slowdown_probability = SLOWDOWN_R;
    test.acceleration_probability = ACC_R;
    test.max_speed = KMPH_TO_CELL_PER_SEC(MAX_SPEED_R);
    test.min_speed = KMPH_TO_CELL_PER_SEC(MIN_SPEED_R);
    test.length = M_TO_CELL(CAR_LENGTH_R);
    test.car_class = default_car;
    test.config = &config;
    EXPECT_EQ (test, cc);
    // cout << test << cc;

    EXPECT_EQ (car_types-1, config.getCarConfig(car_types-1).car_class);
}

// TEST (ConfigTest, LoadFromGABinaryStringAll) {

//     int car_types = 1;
//     int default_car = 0;
//     int genome_length = BITS_TRACK + car_types * BITS_CAR;


//     GA1DBinaryStringGenome genome(genome_length);

//     // HODNOTA                     POCET BITU           POPIS + INTERVAL
//     for (int i = 0; i < genome_length; ++i) {
//         genome.gene(i, 0);
//     }

//     // NASTAVENI TRATE             celkem BITS_TRACK
//     // cell_length                 BITS_CELL_LENGTH     Delka bunky <1, 10>
//     int i = 0;
//     double max = pow(2.0, double(BITS_CELL_LENGTH));
//     cout << "== cell_length: " << BITS_CELL_LENGTH << " bits, max: " << max << endl;
//     for (int n = 0; n < max; ++n) {
//         for (int j = 0; j <= BITS_CELL_LENGTH; ++j) {
//             genome.gene(i + j, n & 1<<j);
//         }

//         // for (int k = 0; k <= BITS_CELL_LENGTH; ++k) {
//         //     cout << genome.gene(k);
//         // }

//         Config config;
//         config.loadFromGABinaryString(genome, default_car);
//         cout << config.getSiteLength() << endl;
//     }
//     i += BITS_CELL_LENGTH;

//     for (int i = 0; i < genome_length; ++i) {
//         genome.gene(i, 0);
//     }
//     // NASTAVENI AUTA              celkem BITS_CAR
//     // car_length                  BITS_CAR_LENGTH            Delka vozidla <5, 20>
//     max = pow(2.0, double(BITS_CAR_LENGTH));
//     cout << "== car_length: " << BITS_CAR_LENGTH << " bits, max: " << max << endl;
//     for (int n = 0; n < max; ++n) {
//         for (int j = 0; j <= BITS_CAR_LENGTH; ++j) {
//             genome.gene(j+i, n & 1<<(j));
//         }

//         Config config;
//         config.loadFromGABinaryString(genome, default_car);
//         CarConfig cc = config.getCarConfig(default_car);
//         cout << cc.length << endl;
//     }
//     i += BITS_CAR_LENGTH;

//     // max_speed                   BITS_MAX_SPEED            Maximalni rychlost vozidla <20, 60>
//     max = pow(2.0, double(BITS_MAX_SPEED));
//     cout << "== max_speed: " << BITS_MAX_SPEED << " bits, max: " << max << endl;
//     for (int n = 0; n < max; ++n) {
//         for (int j = 0; j <= BITS_MAX_SPEED; ++j) {
//             genome.gene(i + j, n & 1<<j);
//         }

//         Config config;
//         config.loadFromGABinaryString(genome, default_car);
//         CarConfig cc = config.getCarConfig(default_car);
//         cout << cc.max_speed << endl;
//     }
//     i += BITS_MAX_SPEED;

//     // min_speed                   BITS_MIN_SPEED            Minimalni rychlost vozidla <0, 20>
//     max = pow(2.0, double(BITS_MIN_SPEED));
//     cout << "== min_speed: " << BITS_MIN_SPEED << " bits, max: " << max << endl;
//     for (int n = 0; n < max; ++n) {
//         for (int j = 0; j <= BITS_MIN_SPEED; ++j) {
//             genome.gene(i + j, n & 1<<j);
//         }

//         Config config;
//         config.loadFromGABinaryString(genome, default_car);
//         CarConfig cc = config.getCarConfig(default_car);
//         cout << cc.min_speed << endl;
//     }
//     i += BITS_MIN_SPEED;

//     // slowdown_probability        BITS_SLOWDOWN_P            Pravdepodobnost zpomaleni <0, 1>
//     max = pow(2.0, double(BITS_SLOWDOWN_P));
//     cout << "== slowdown_probability: " << BITS_SLOWDOWN_P << " bits, max: " << max << endl;
//     for (int n = 0; n < max; ++n) {
//         for (int j = 0; j <= BITS_SLOWDOWN_P; ++j) {
//             genome.gene(i + j, n & 1<<j);
//         }

//         Config config;
//         config.loadFromGABinaryString(genome, default_car);
//         CarConfig cc = config.getCarConfig(default_car);
//         cout << cc.slowdown_probability << endl;
//     }
//     i += BITS_SLOWDOWN_P;

//     // acceleration_probability    BITS_ACC_P            Pravdepodobnost zrychleni <0, 1>
//     max = pow(2.0, double(BITS_ACC_P));
//     cout << "== acceleration_probability: " << BITS_ACC_P << " bits, max: " << max << endl;
//     for (int n = 0; n < max; ++n) {
//         for (int j = 0; j <= BITS_ACC_P; ++j) {
//             genome.gene(i + j, n & 1<<j);
//         }

//         Config config;
//         config.loadFromGABinaryString(genome, default_car);
//         CarConfig cc = config.getCarConfig(default_car);
//         cout << cc.acceleration_probability << endl;
//     }
// }

TEST (ConfigTest, LoadFromGABinaryStringMinValues) {

     int car_types = 6;
     int default_car = 1;
     int genome_length = BITS_TRACK + car_types * BITS_CAR;

    GA1DBinaryStringGenome genome(genome_length);

    for (int i = 0; i < genome_length; ++i) {
        genome.gene(i, 0);
    }

    Config config;
    config.loadFromGABinaryString(genome, default_car);
    int track_length = 5000;
    float site_length = CELL_LENGTH_L;
    config.setTrackLength(track_length);

    EXPECT_EQ (site_length, config.getSiteLength());
    EXPECT_EQ (track_length, config.getTrackLength());
    EXPECT_EQ (default_car, config.getDefaultCar());
    EXPECT_EQ (int(track_length/site_length), config.getNumberOfTrackCells());
    EXPECT_EQ (car_types, config.getNumberOfCarTypes());

    CarConfig cc = config.getCarConfig(default_car);
    EXPECT_EQ (cc, config.getCarConfig(-1));

    CarConfig test;
    test.slowdown_probability = SLOWDOWN_L;
    test.acceleration_probability = ACC_L;
    test.max_speed = KMPH_TO_CELL_PER_SEC(MAX_SPEED_L);
    test.min_speed = KMPH_TO_CELL_PER_SEC(MIN_SPEED_L);
    test.length = M_TO_CELL(CAR_LENGTH_L);
    test.car_class = default_car;
    test.config = &config;
    EXPECT_EQ (test, cc);
    // cout << "Expected:\n" << test << "Actual:\n" << cc;

    EXPECT_EQ (car_types-1, config.getCarConfig(car_types-1).car_class);
}

// Tento test lze vyuzit pro prevod genomu z podoby binarniho retezce
// do formatu, ktery lze pouzit pro nastaveni simulatoru
// TEST (ConfigTest, genomeToConfig) {
//     string str("0110100101010111000100000101010100000110001110100000000100010111100011110100000101010111001011101011110111111100011110100000100100010001011011010011010000110000");

//     GA1DBinaryStringGenome genome(str.size());

//     for (int i=0; i < str.length(); ++i)
//         genome.gene(i, int(str[i]) - 48);

//     Config config;
//     config.loadFromGABinaryString(genome, 1);

//     config.print();
// }
