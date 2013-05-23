/*
 * Datum:   20.5.2013
 * Autor:   Pavel Pokorny, xpokor12@stud.fit.vutbr.cz
 * Projekt: DP - Kalibrace mikrosimulacniho modelu dopravy
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include <fstream>
#include <map>

// Nastaveni poctu bitu jednotlivych konfiguraci
#define BITS_CELL_LENGTH 6

#define BITS_CAR_LENGTH 6
#define BITS_MAX_SPEED 6
#define BITS_MIN_SPEED 4
#define BITS_SLOWDOWN_P 5
#define BITS_ACC_P 5

// pocet bitu potrebny pro ulozeni konfigurace trate
#define BITS_TRACK (BITS_CELL_LENGTH)
// pocet bitu potrebny pro ulozeni konfigurace jednoho vozidla
#define BITS_CAR (BITS_CAR_LENGTH + BITS_MAX_SPEED + BITS_MIN_SPEED + BITS_SLOWDOWN_P + BITS_ACC_P)

// krajni hodnoty jednotlivych nastaveni
#define CELL_LENGTH_L 0.3
#define CELL_LENGTH_R 16.0

#define CAR_LENGTH_L 5
#define CAR_LENGTH_R 20

#define MAX_SPEED_L 20
#define MAX_SPEED_R 70

#define MIN_SPEED_L 0
#define MIN_SPEED_R 10

#define SLOWDOWN_L 0.0
#define SLOWDOWN_R 0.6

#define ACC_L 0.3
#define ACC_R 1.0

// makro pro vygenerovani definice, getteru a setteru parametru
#define CONFIG_ITEM(type, name, canonical) \
  protected: type canonical; \
  public: \
    type get##name() { return canonical; } \
    void set##name(type value) { this->canonical = value; }

class GABinaryString;
class Config;

struct CarConfig {

  int car_class;
  float slowdown_probability;
  float acceleration_probability;
  int max_speed;
  int min_speed;
  int length;
  Config * config;

  bool operator== (const CarConfig& cc) const {
    return cc.slowdown_probability == slowdown_probability &&
      cc.acceleration_probability == acceleration_probability &&
      cc.max_speed == max_speed &&
      cc.min_speed == min_speed &&
      cc.length == length;
  }

  std::string toString();

  void print();
};

std::ostream& operator<<(std::ostream& out, CarConfig& r);

class Config {

  CONFIG_ITEM(float, SiteLength, site_length)
  CONFIG_ITEM(int, TrackLength, track_length)
  CONFIG_ITEM(int, DefaultCar, default_car)
  CONFIG_ITEM(bool, TableFormat, table_format)
  CONFIG_ITEM(int, StatsFrequency, stats_frequency)
  CONFIG_ITEM(int, CarFactory, car_factory)
  CONFIG_ITEM(bool, PeriodicBoundary, periodic_boundary)
  CONFIG_ITEM(int, SCFInterval, scf_interval)
  CONFIG_ITEM(float, NCFDeviation, ncf_deviation)
  CONFIG_ITEM(int, NCFMean, ncf_mean)
  CONFIG_ITEM(char *, SamplesFile, samples_file)
  CONFIG_ITEM(unsigned long, MaxTime, max_time)
  CONFIG_ITEM(bool, SlowToStop, slow_to_stop)
  CONFIG_ITEM(float, SlowToStartProbability, slow_to_start_probability)
  CONFIG_ITEM(bool, TrueSlowdown, true_slowdown)

  std::map <int, CarConfig> car_configs;

  public:
    Config() {
      // nastaveni vychozich hodnot
      track_length = 5350;
      default_car = 0;
      site_length = 1;
      table_format = true;
      stats_frequency = 0;
      car_factory = 2;
      periodic_boundary = false;
      ncf_mean = 20;
      ncf_deviation = 10;
      scf_interval = 1;
      max_time = 0;
      slow_to_stop = false;
      slow_to_start_probability = 0.5;
      samples_file = NULL;
      true_slowdown = false;
    }
    ~Config();

    /**
     * Pouziti techto metod lze videt v tests/config_test.cc
     */
    int loadFromFile(const char *filename);
    void loadFromInteger(int binary_integer);

    /**
     * Nacteni konfigurace z GAlib GABinStr, lze pouzit pouze v pripade,
     * ze je nalinkovana knihovna GAlib.
     */
    void loadFromGABinaryString(GABinaryString& str, int default_car);

    CarConfig getCarConfig(int car_class);
    CarConfig getDefaultCarConfig();

    void print();

    int getNumberOfTrackCells() { return site_length > 0 ? int(track_length/site_length) : 1; }
    int getNumberOfCarTypes() { return car_configs.size(); }
};

#endif
