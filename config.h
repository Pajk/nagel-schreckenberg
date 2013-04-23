#ifndef _CONFIG_H
#define _CONFIG_H

#include <fstream>
#include <map>

struct CarConfig {
  float slowdown_probability;
  float acceleration_probability;
  int max_speed;
  int min_speed;
  bool operator== (const CarConfig& cc) const {
    return cc.slowdown_probability == slowdown_probability &&
      cc.acceleration_probability == acceleration_probability &&
      cc.max_speed == max_speed &&
      cc.min_speed == min_speed;
  }
};

class Config {
   
  int site_length;
  int track_length;
  int default_car;

  std::map <int, CarConfig> car_configs;

  public:
    Config();

    void loadFromFile(const char *filename);
    void loadFromBinaryString(const char *binary_string);
    void loadFromInteger(int binary_integer);
    CarConfig getCarConfig(int car_class);
    CarConfig getDefaultCarConfig();

    void print();

    int getSiteLength() { return site_length; }
    int getTrackLength() { return track_length; }
    int getDefaultCar() { return default_car; }
    int getNumberOfTrackSites() { return site_length > 0 ? track_length/site_length : 1; }
    int getNumberOfCarTypes() { return car_configs.size(); }
};


#endif

