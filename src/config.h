#ifndef _CONFIG_H
#define _CONFIG_H

#include <fstream>
#include <map>

// Nastaveni poctu bitu jednotlivych konfiguraci
#define BITS_CELL_LENGTH 4

#define BITS_CAR_LENGTH 4
#define BITS_MAX_SPEED 5
#define BITS_MIN_SPEED 4
#define BITS_SLOWDOWN_P 7
#define BITS_ACC_P 7

// pocet bitu potrebny pro ulozeni konfigurace trate
#define BITS_TRACK (BITS_CELL_LENGTH)
// pocet bitu potrebny pro ulozeni konfigurace jednoho vozidla
#define BITS_CAR (BITS_CAR_LENGTH + BITS_MAX_SPEED + BITS_MIN_SPEED + BITS_SLOWDOWN_P + BITS_ACC_P)

// krajni hodnoty jednotlivych nastaveni
#define CELL_LENGTH_L 1
#define CELL_LENGTH_R 10

#define CAR_LENGTH_L 5
#define CAR_LENGTH_R 20

#define MAX_SPEED_L 10
#define MAX_SPEED_R 120

#define MIN_SPEED_L 0
#define MIN_SPEED_R 10

#define SLOWDOWN_L 0.0
#define SLOWDOWN_R 0.8

#define ACC_L 0.3
#define ACC_R 1.0

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

  float site_length;
  int track_length;
  int default_car;
  bool table_format;
  int stats_frequency;
  int car_factory;
  bool periodic_boundary;
  int scf_interval;
  int ncf_mean;
  float ncf_deviation;
  unsigned long max_time;

  std::map <int, CarConfig> car_configs;

  public:
    Config();

    /**
     * Pouziti techto medot lze videt v tests/config_test.cc
     */
    void loadFromFile(const char *filename);
    void loadFromInteger(int binary_integer);

    /**
     * Nacteni konfigurace z GAlib GABinStr, lze pouzit pouze v pripade,
     * ze je nalinkovana knihovna GAlib.
     */
    void loadFromGABinaryString(GABinaryString& str, int default_car);

    CarConfig getCarConfig(int car_class);
    CarConfig getDefaultCarConfig();

    void print();

    float getSiteLength() { return site_length; }
    int getTrackLength() { return track_length; }
    int getDefaultCar() { return default_car; }
    int getNumberOfTrackCells() { return site_length > 0 ? track_length/site_length : 1; }
    int getNumberOfCarTypes() { return car_configs.size(); }
    bool useTableFormat() { return table_format; }
    int getStatsFrequency() { return stats_frequency; }
    int getCarFactory() { return car_factory; }
    bool usePeriodicBoundary() { return periodic_boundary; }
    int getSCFInterval() { return scf_interval; }
    int getNCFMean() { return ncf_mean; }
    int getNCFDeviation() { return ncf_deviation; }
    unsigned long getMaxTime() { return max_time; }

    void setTrackLength(int track_length) { this->track_length = track_length; }
    void setSiteLength(float site_length) { this->site_length = site_length; }
    void setTableFormat(bool table_format) { this->table_format = table_format; }
};

#endif
