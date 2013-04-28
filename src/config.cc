#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cstdlib>

#include "config.h"

using namespace std;

Config::Config() {
  track_length = 5350;
  default_car = 0;
  site_length = 2.5;
  track_max_speed = 6;
}

void Config::loadFromFile(const char *filename) {

  ifstream fin(filename);

  if (!fin.good()) {
    cout << "Cannot open config file '" << filename << "'." << endl;
    exit(-1);
  }

  string line;
  size_t first_char;

  while(getline(fin, line)) {
    istringstream sin(line.substr(line.find("=") + 1));

    if (line.empty()) {
      continue;
    }

    // kontrola zda nejde o komentar
    first_char = line.find_first_not_of(" \t");
    if (first_char != string::npos && line.at(first_char) == '#') {
      continue;
    }

    if (line.find("site_length") != string::npos) {
      sin >> site_length;

    } else if (line.find("track_length") != string::npos) {
      sin >> track_length;

    } else if (line.find("track_max_speed") != string::npos) {
      sin >> track_max_speed;

    } else if (line.find("default_car") != string::npos) {
      sin >> default_car;

    } else if (line.find("car ") == 0) {

      istringstream csin(line.substr(4));

      CarConfig car_config;

      csin >> car_config.car_class;

      while(getline(fin, line)) {

        istringstream csin(line.substr(line.find("=") + 1));

        // preskoceni komentare
        first_char = line.find_first_not_of(" \t");
        if (first_char != string::npos && line.at(first_char) == '#') {
          continue;
        }

        if (line.find("endcar") == 0) {
          car_configs[car_config.car_class] = car_config;
          break;

        } else if (line.find("slowdown_probability", 2) != string::npos) {
          csin >> car_config.slowdown_probability;

        } else if (line.find("acceleration_probability", 2) != string::npos) {
          csin >> car_config.acceleration_probability;

        } else if (line.find("max_speed", 2) != string::npos) {
          csin >> car_config.max_speed;

        } else if (line.find("min_speed", 2) != string::npos) {
          csin >> car_config.min_speed;

        } else if (line.find("length", 2) != string::npos) {
          csin >> car_config.length;
        }
      }
    }
  }
}


CarConfig Config::getCarConfig(int car_class) {

  map<int, CarConfig>::iterator it = car_configs.find(car_class);

  if (it == car_configs.end()) {
    return car_configs[default_car];
  } else {
    return car_configs[car_class];
  }
}

CarConfig Config::getDefaultCarConfig() {

  return car_configs[default_car];

}

void Config::print() {

    cout << "site length: " << site_length << endl
         << "track length: " << track_length << endl
         << "number of track sites: " << getNumberOfTrackCells() << endl;

    for (map<int,CarConfig>::iterator it = car_configs.begin(); it != car_configs.end(); ++it) {
        cout << "car " << it->first << ":" << endl;
        CarConfig cc = it->second;
        cout << "  slowdown_probability: " << cc.slowdown_probability << endl
             << "  acceleration_probability: " << cc.acceleration_probability << endl
             << "  max_speed: " << cc.max_speed << endl
             << "  min_speed: " << cc.min_speed << endl
             << "  length: " << cc.length << endl;
    }
}

void Config::loadFromInteger(int binary_integer) {

  int i, tmp;

  CarConfig car_config;
  car_config.min_speed = 1;
  car_config.car_class = 0;

  // format: car_length[3]; max_speed[4]; slowdown_probability[7]; acceleration_probability[7];
  // 21 bitu
  // BITY   HODNOTA                     POCET BITU

  // Delka vozidla <1,8>
  // 20-18  car_length                  3

  for (i = 20, tmp = 0; i >= 18; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<(i-18);
    }
  }
  tmp += 1; // <0,7> => <1,8>
  car_config.length = tmp;

  // Maximalni rychlost vozidla <1,16>
  // 17-14  max_speed                   4

  for (i = 17, tmp = 0; i >= 14; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<(i-14);
    }
  }
  tmp += 1;  // <0,15> => <1,16>
  car_config.max_speed = tmp;

  // Pravdepodobnost zpomaleni <0,1.0>
  // 13-7   slowdown_probability        7

  for (i = 13, tmp = 0; i >= 7; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<(i-7);
    }
  }
  // v teto fazi je hodnota tmp v intervalu <0,127>
  // nasleduje prevod do intrvalu <0,1.0>
  tmp += 1;
  float unit = 1.0/128;
  car_config.slowdown_probability = float(tmp) * unit;

  // Pravdepodobnost zrychleni <0, 1>
  // 6-0    acceleration_probability    7
  for (i = 6, tmp = 0; i >= 0; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<i;
    }
  }
  tmp += 1;
  car_config.acceleration_probability = float(tmp) * unit;

  car_configs[0] = car_config;
}


void CarConfig::print() {
  cout << "=== car config: " << endl
       << "class: " << car_class << endl
       << "slowdown_probability: " << slowdown_probability << endl
       << "acceleration_probability: " << acceleration_probability << endl
       << "max_speed: " << max_speed << endl
       << "min_speed: " << min_speed << endl
       << "length: " << length << endl;
}
