#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cstdlib>
#include <cmath>
#include <ga/GABinStr.h>

#include "config.h"

using namespace std;

Config::Config() {
  track_length = 5350;
  default_car = 0;
  site_length = 2.5;
  track_max_speed = 50;
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
      car_config.config = this;

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
          car_config.max_speed = roundf(float(car_config.max_speed) / 3.6 / site_length);

        } else if (line.find("min_speed", 2) != string::npos) {
          csin >> car_config.min_speed;
          car_config.min_speed = roundf(float(car_config.min_speed) / 3.6 / site_length);

        } else if (line.find("length", 2) != string::npos) {
          csin >> car_config.length;
          car_config.length = roundf(float(car_config.length) / site_length);
        }
      }
    }
  }

  // prevod max rychlosti z km/h na bunky/s
  track_max_speed = round(float(track_max_speed) / 3.6 / site_length);
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
        CarConfig cc = it->second;
        cc.print();
    }
}

void Config::loadFromInteger(int binary_integer) {

  int i, tmp;
  float unit;

  CarConfig car_config;
  car_config.config = this;
  car_config.min_speed = 0;
  car_config.car_class = 0;

  // popis formatu je uveden v config_test.cc

  // BITY   HODNOTA                     POCET BITU   POPIS + INTERVAL
  // 25-22  cell_length                 4            Delka bunky <1, 10>
  for (i = 25, tmp = 0; i >= 22; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<(i-22);
    }
  }
  tmp += 1;
  unit = 10.0/16.0;
  site_length = float(tmp) * unit;

  // 21-18  car_length                  4            Delka vozidla <5, 20>

  for (i = 21, tmp = 0; i >= 18; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<(i-18);
    }
  }
  car_config.length = roundf(float(5 + tmp)/site_length);

  // 17-14  max_speed                   4            Maximalni rychlost vozidla <20, 60>

  for (i = 17, tmp = 0; i >= 14; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<(i-14);
    }
  }
  tmp += 1;  // <0,15> => <1,16>
  unit = 40.0/16.0;
  car_config.max_speed = roundf(float(20 + tmp*unit) / 3.6 / site_length);

  // 13-7   slowdown_probability        7            Pravdepodobnost zpomaleni <0, 1>

  for (i = 13, tmp = 0; i >= 7; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<(i-7);
    }
  }
  // v teto fazi je hodnota tmp v intervalu <0,127>
  // nasleduje prevod do intrvalu <0,1.0>
  tmp += 1;
  unit = 1.0/128;
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

void Config::loadFromGABinaryString(GABinaryString& str) {

  int i, tmp;
  float unit;

  CarConfig car_config;
  car_config.config = this;
  car_config.min_speed = 0;
  car_config.car_class = 0;

  // popis formatu je uveden v config_test.cc

  // BITY   HODNOTA                     POCET BITU   POPIS + INTERVAL
  // 25-22  cell_length                 4            Delka bunky <1, 10>
  for (i = 25, tmp = 0; i >= 22; i--) {
    if (str.bit(i)) {
        tmp |= 1<<(i-22);
    }
  }
  tmp += 1;
  unit = 10.0/16.0;
  site_length = float(tmp) * unit;

  // 21-18  car_length                  4            Delka vozidla <5, 20>

  for (i = 21, tmp = 0; i >= 18; i--) {
    if (str.bit(i)) {
        tmp |= 1<<(i-18);
    }
  }
  car_config.length = roundf(float(5 + tmp)/site_length);

  // 17-14  max_speed                   4            Maximalni rychlost vozidla <20, 60>

  for (i = 17, tmp = 0; i >= 14; i--) {
    if (str.bit(i)) {
        tmp |= 1<<(i-14);
    }
  }
  tmp += 1;  // <0,15> => <1,16>
  unit = 40.0/16.0;
  car_config.max_speed = roundf(float(20 + tmp*unit) / 3.6 / site_length);

  // 13-7   slowdown_probability        7            Pravdepodobnost zpomaleni <0, 1>

  for (i = 13, tmp = 0; i >= 7; i--) {
    if (str.bit(i)) {
        tmp |= 1<<(i-7);
    }
  }
  // v teto fazi je hodnota tmp v intervalu <0,127>
  // nasleduje prevod do intrvalu <0,1.0>
  tmp += 1;
  unit = 1.0/128;
  car_config.slowdown_probability = float(tmp) * unit;

  // Pravdepodobnost zrychleni <0, 1>
  // 6-0    acceleration_probability    7
  for (i = 6, tmp = 0; i >= 0; i--) {
    if (str.bit(i)) {
        tmp |= 1<<i;
    }
  }
  tmp += 1;
  car_config.acceleration_probability = float(tmp) * unit;

  car_configs[0] = car_config;
}


void CarConfig::print() {
  cout << "= car " << car_class << ": " << endl
       << "  slowdown_probability: " << slowdown_probability << endl
       << "  acceleration_probability: " << acceleration_probability << endl
       << "  max_speed: " << max_speed << " (" <<
        max_speed * config->getSiteLength() * 3.6 << " km/h)" << endl
       << "  min_speed: " << min_speed << " (" <<
        min_speed * config->getSiteLength() * 3.6 << " km/h)"<< endl
       << "  length: " << length << " (" <<
        length * config->getSiteLength() << " m)" << endl;
}
