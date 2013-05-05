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

    cout << "site_length = " << site_length << endl
         << "track_length = " << track_length << endl
         << "# number of track sites = " << getNumberOfTrackCells() << endl
         << "default_car = " << default_car << endl;

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

void Config::loadFromGABinaryString(GABinaryString& str, int default_car) {

  unsigned int i, tmp, position = 0;
  float unit;

  this->default_car = default_car;

  // HODNOTA                     POCET BITU   POPIS + INTERVAL
  // cell_length                 4            Delka bunky <1, 10>
  for (i = 0, tmp = 0; i < 4; ++i, ++position) {
    tmp |= str.bit(position) << i;
  }
  tmp += 1;
  unit = 10.0/16.0;
  site_length = float(tmp) * unit;


  // Odvozeni poctu typu vozidel z delky genomu
  int car_types = (str.size() - position)/26;

  // nacteni konfiguraci jednotlivych typu vozidel
  for (int car_class = 0; car_class < car_types; ++car_class) {

    CarConfig car_config;
    car_config.config = this;
    car_config.car_class = car_class;

    /**
     * car_length                  4            Delka vozidla <5, 20>
     */
    for (i = 0, tmp = 0; i < 4; ++i, ++position) {
      tmp |= str.bit(position) << i;
    }
    car_config.length = roundf(float(5 + tmp)/site_length);

    /**
     * max_speed                   4            Maximalni rychlost vozidla <20, 60>
     */
    for (i = 0, tmp = 0; i < 4; ++i, ++position) {
      tmp |= str.bit(position) << i;
    }
    // tmp += 1;  // <0,15> => <1,16>
    unit = 40.0/15.0;
    car_config.max_speed = roundf(float(20.0 + tmp*unit) / 3.6 / site_length);

    /**
     * min_speed                   4            Minimalni rychlost vozidla <0, 20>
     */
    for (i = 0, tmp = 0; i < 4; ++i, ++position) {
      tmp |= str.bit(position) << i;
    }
    unit = 20.0/15.0;
    car_config.min_speed = roundf(float(tmp*unit) / 3.6 / site_length);

    /**
     * slowdown_probability        7            Pravdepodobnost zpomaleni <0, 0.5>
     */
    for (i = 0, tmp = 0; i < 7; ++i, ++position) {
      tmp |= str.bit(position) << i;
    }
    // v teto fazi je hodnota tmp v intervalu <0,127>
    // nasleduje prevod do intrvalu <0,0.5>
    unit = 0.5/127;
    car_config.slowdown_probability = float(tmp) * unit;

    /**
     * acceleration_probability    7            Pravdepodobnost zrychleni <0.1, 1>
     */
    for (i = 0, tmp = 0; i < 7; ++i, ++position) {
      tmp |= str.bit(position) << i;
    }
    unit = 1.0/128;
    tmp += 1;
    car_config.acceleration_probability = float(tmp) * unit;

    car_configs[car_class] = car_config;

    // car_config.print();
  }

}

std::ostream& operator<<(std::ostream& out, CarConfig& r) {
  return out<< r.toString();
}

string CarConfig::toString() {
  stringstream ss;

  ss << "car " << car_class << endl
       << "  slowdown_probability = " << slowdown_probability << endl
       << "  acceleration_probability = " << acceleration_probability << endl
       << "  max_speed = " << max_speed * config->getSiteLength() * 3.6 << " (" <<
         max_speed << " cell/s)" << endl
       << "  min_speed = " << min_speed * config->getSiteLength() * 3.6 << " (" <<
        min_speed << " cell/s)"<< endl
       << "  length = " << length * config->getSiteLength() << " (" <<
         length << " cells)" << endl << "endcar" << endl;

  return ss.str();
}


void CarConfig::print() {
  cout << toString();
}
