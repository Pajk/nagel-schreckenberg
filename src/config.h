#ifndef _CONFIG_H
#define _CONFIG_H

#include <fstream>
#include <map>

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

    void setTrackLength(int track_length) {
      this->track_length = track_length;
    }
};

#endif
