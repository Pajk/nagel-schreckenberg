#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cstdlib>
#include <cmath>
#include <ga/GABinStr.h>

#include "config.h"

using namespace std;


#define STR(s) #s
#define LOAD_CAR_CONFIG(opt) \
  if (line.find(STR(opt =), 2) != string::npos) { \
    csin >> car_config.opt; \
  }
#define LOAD_CONFIG(opt) \
  if (line.find(STR(opt =)) != string::npos) { \
    sin >> opt; loaded++; \
  } else

Config::~Config() {
  if (samples_file != NULL) {
    delete[] samples_file;
  }
}

int Config::loadFromFile(const char *filename) {

  ifstream fin(filename);

  if (!fin.good()) {
    cerr << "Cannot open config file '" << filename << "'." << endl;
    return 0;
  }

  string line;
  size_t first_char;
  int loaded = 0;

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

    LOAD_CONFIG(site_length)
    LOAD_CONFIG(track_length)
    LOAD_CONFIG(default_car)
    LOAD_CONFIG(car_factory)
    LOAD_CONFIG(scf_interval)
    LOAD_CONFIG(ncf_mean)
    LOAD_CONFIG(ncf_deviation)
    LOAD_CONFIG(periodic_boundary)
    LOAD_CONFIG(table_format)
    LOAD_CONFIG(stats_frequency)
    LOAD_CONFIG(max_time)
    LOAD_CONFIG(slow_to_stop)
    LOAD_CONFIG(slow_to_start_probability)
    LOAD_CONFIG(true_slowdown)
    if (line.find("samples_file") != string::npos) {

      if (samples_file == NULL) samples_file = new char[255];

      sin >> samples_file;

    } else

    if (line.find("car ") == 0) {

      istringstream csin(line.substr(4));
      CarConfig car_config;
      car_config.config = this;
      csin >> car_config.car_class;

      while(getline(fin, line)) {

        istringstream csin(line.substr(line.find("=") + 1));

        // preskoceni komentare
        first_char = line.find_first_not_of(" \t");
        if (first_char != string::npos && line.at(first_char) == '#') { continue; }

        // nacteni hodnot
        if (line.find("endcar") == 0) {

          // prepocet na interni jednotky
          car_config.max_speed = roundf(float(car_config.max_speed) / 3.6 / site_length);
          car_config.min_speed = roundf(float(car_config.min_speed) / 3.6 / site_length);
          car_config.length = roundf(float(car_config.length) / site_length);
          if (car_config.length == 0) car_config.length = 1;

          // auto musi mit moznost se pohybovat
          if (car_config.max_speed == 0) car_config.max_speed = 1;

          // ulozeni nacteneho nastaveni vozidla
          car_configs[car_config.car_class] = car_config;

          break;
        } else
        LOAD_CAR_CONFIG(slowdown_probability) else
        LOAD_CAR_CONFIG(acceleration_probability) else
        LOAD_CAR_CONFIG(max_speed) else
        LOAD_CAR_CONFIG(min_speed) else
        LOAD_CAR_CONFIG(length)
      }
    }
  }

  // uprava nactenych hodnot
  stats_frequency *= 60;
  max_time *= 3600;

  return loaded;
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

  cout << "car_factory = " << car_factory << endl
       << "scf_interval = " << scf_interval << endl
       << "ncf_mean = " << ncf_mean << endl
       << "ncf_deviation = " << ncf_deviation << endl;
  if (samples_file != NULL) {
       cout << "samples_file = " << samples_file << endl;
  }
  cout << "max_time = " << max_time/3600 << endl
       << "periodic_boundary = " << periodic_boundary << endl
       << "site_length = " << site_length << endl
       << "track_length = " << track_length << endl
       << "# number of track sites = " << getNumberOfTrackCells() << endl
       << "default_car = " << default_car << endl
       << "table_format = " << table_format << endl
       << "stats_frequency = " << stats_frequency/60 << endl
       << "slow_to_stop = " << slow_to_stop << endl
       << "slow_to_start_probability = " << slow_to_start_probability << endl
       << "true_slowdown = " << true_slowdown << endl;

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
  if (car_config.length == 0) car_config.length = 1;

  // 17-14  max_speed                   4            Maximalni rychlost vozidla <20, 60>

  for (i = 17, tmp = 0; i >= 14; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<(i-14);
    }
  }
  tmp += 1;  // <0,15> => <1,16>
  unit = 40.0/16.0;
  car_config.max_speed = roundf(float(20 + tmp*unit) / 3.6 / site_length);
  if(car_config.max_speed == 0) car_config.max_speed = 1;

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

  // HODNOTA        POCET BITU          POPIS + INTERVAL
  // cell_length    BITS_CELL_LENGTH    Delka bunky <CELL_LENGTH_L, CELL_LENGTH_R>
  for (i = 0, tmp = 0; i < BITS_CELL_LENGTH; ++i, ++position) {
    tmp |= str.bit(position) << i;
  }
  unit = (CELL_LENGTH_R-CELL_LENGTH_L)/(pow(2.0,BITS_CELL_LENGTH)-1);
  site_length = CELL_LENGTH_L + float(tmp) * unit;

  // Odvozeni poctu typu vozidel z delky genomu
  int car_types = (str.size() - position)/BITS_CAR;

  // nacteni konfiguraci jednotlivych typu vozidel
  for (int car_class = 0; car_class < car_types; ++car_class) {

    CarConfig car_config;
    car_config.config = this;
    car_config.car_class = car_class;

    /**
     * car_length   BITS_CAR_LENGTH   Delka vozidla <CAR_LENGTH_L, CAR_LENGTH_R>
     */
    for (i = 0, tmp = 0; i < BITS_CAR_LENGTH; ++i, ++position) {
      tmp |= str.bit(position) << i;
    }
    unit = (CAR_LENGTH_R-CAR_LENGTH_L)/(pow(2.0,BITS_CAR_LENGTH)-1);
    // cout << unit << " " << (CAR_LENGTH_L + float(tmp) * unit)/site_length << " ";
    car_config.length = roundf((CAR_LENGTH_L + float(tmp) * unit)/site_length);
    // cout << car_config.length << endl;
    if (car_config.length == 0) car_config.length = 1;

    /**
     * max_speed    BITS_MAX_SPEED    Maximalni rychlost vozidla <MAX_SPEED_L, MAX_SPEED_R>
     */
    for (i = 0, tmp = 0; i < BITS_MAX_SPEED; ++i, ++position) {
      tmp |= str.bit(position) << i;
    }
    unit = (MAX_SPEED_R-MAX_SPEED_L)/(pow(2.0,BITS_MAX_SPEED)-1);
    car_config.max_speed = roundf(float(MAX_SPEED_L + tmp * unit) / 3.6 / site_length);
    if(car_config.max_speed == 0) car_config.max_speed = 1;

    /**
     * min_speed    BITS_MIN_SPEED    Minimalni rychlost vozidla <MIN_SPEED_L, MIN_SPEED_R>
     */
    for (i = 0, tmp = 0; i < BITS_MIN_SPEED; ++i, ++position) {
      tmp |= str.bit(position) << i;
    }
    unit = (MIN_SPEED_R-MIN_SPEED_L)/(pow(2.0,BITS_MIN_SPEED)-1);
    car_config.min_speed = roundf(float(MIN_SPEED_L + tmp * unit) / 3.6 / site_length);

    /**
     * slowdown_probability   BITS_SLOWDOWN_P   Pravdepodobnost zpomaleni <SLOWDOWN_L, SLOWDOWN_P>
     */
    for (i = 0, tmp = 0; i < BITS_SLOWDOWN_P; ++i, ++position) {
      tmp |= str.bit(position) << i;
    }
    unit = (SLOWDOWN_R-SLOWDOWN_L)/(pow(2.0,BITS_SLOWDOWN_P)-1);
    car_config.slowdown_probability = SLOWDOWN_L + tmp * unit;

    /**
     * acceleration_probability    BITS_ACC_P   Pravdepodobnost zrychleni <ACC_L, ACC_R>
     */
    for (i = 0, tmp = 0; i < BITS_ACC_P; ++i, ++position) {
      tmp |= str.bit(position) << i;
    }
    unit = (ACC_R-ACC_L)/(pow(2.0,BITS_ACC_P)-1);
    car_config.acceleration_probability = ACC_L + tmp * unit;

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
