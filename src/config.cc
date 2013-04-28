#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cstdlib>

#include "config.h"

using namespace std;

Config::Config() {
  track_length = 5400;
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

void Config::loadFromBinaryString(const char *binary_string) {

  int i, tmp;
  const char *config = binary_string;

  for (i = 3, tmp = 0; i >= 0; i--) {
    if (*config == '1') {
       tmp |= 1<<i;
    }
    config++;
  }
  site_length = tmp;

  CarConfig car_config;

  for (i = 6, tmp = 0; i >= 0; i--) {
    if (*config == '1') {
        tmp |= 1<<i;
    }
    config++;
  }
  car_config.slowdown_probability = float(tmp)/100.0;

  for (i = 6, tmp = 0; i >= 0; i--) {
    if (*config == '1') {
        tmp |= 1<<i;
    }
    config++;
  }
  car_config.acceleration_probability = float(tmp)/100.0;

  for (i = 3, tmp = 0; i >= 0; i--) {
    if (*config == '1') {
        tmp |= 1<<i;
    }
    config++;
  }
  car_config.max_speed = tmp;

  for (i = 3, tmp = 0; i >= 0; i--) {
    if (*config == '1') {
        tmp |= 1<<i;
    }
    config++;
  }
  car_config.min_speed = tmp;

  car_configs[0] = car_config;
}

void Config::print() {

    cout << "site length: " << site_length << endl
         << "track length: " << track_length << endl
         << "number of track sites: " << getNumberOfTrackSites() << endl;

    for (map<int,CarConfig>::iterator it = car_configs.begin(); it != car_configs.end(); ++it) {
        cout << "car " << it->first << ":" << endl;
        CarConfig cc = it->second;
        cout << "  slowdown_probability: " << cc.slowdown_probability << endl
             << "  acceleration_probability: " << cc.acceleration_probability << endl
             << "  max_speed: " << cc.max_speed << endl
             << "  min_speed: " << cc.min_speed << endl;
    }
}

void Config::loadFromInteger(int binary_integer) {

  int i, tmp;

  for (i = 25, tmp = 0; i >= 22; i--) {
    if (binary_integer>>i & 1) {
       tmp |= 1<<(i-22);
    }
  }
  site_length = tmp;

  CarConfig car_config;

  for (i = 21, tmp = 0; i >= 15; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<(i-15);
    }
  }
  if (tmp > 60) tmp = 60;
  if (tmp < 10) tmp = 10;
  car_config.slowdown_probability = float(tmp)/100.0;

  for (i = 14, tmp = 0; i >= 8; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<(i-8);
    }
  }
  if (tmp > 100) tmp = 100;
  if (tmp < 10) tmp = 10;
  car_config.acceleration_probability = float(tmp)/100.0;

  for (i = 7, tmp = 0; i >= 4; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<(i-4);
    }
  }
  if (tmp < 2) tmp = 2;
  car_config.max_speed = tmp;

  for (i = 3, tmp = 0; i >= 0; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<i;
    }
  }
  if (tmp >= car_config.max_speed) {
    tmp = car_config.max_speed - 2;
  }
  if (tmp < 0) tmp = 0;
  car_config.min_speed = tmp;


  car_config.length = 3;

  car_configs[0] = car_config;
}
