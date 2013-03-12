#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include "config.h"

using namespace std;

Config::Config() {
  track_length = 5400;
  default_car = 0;
}

void Config::loadFromFile(const char *filename) {

  ifstream fin(filename);

  string line;
  int car_class;
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

      csin >> car_class;
      CarConfig car_config;

      while(getline(fin, line)) {

        istringstream csin(line.substr(line.find("=") + 1));

        first_char = line.find_first_not_of(" \t");
        if (first_char != string::npos && line.at(first_char) == '#') {
          continue;     
        }

        if (line.find("endcar") == 0) {
          car_configs[car_class] = car_config;
          break;
        } else if (line.find("slowdown_probability", 2) != string::npos) {
          csin >> car_config.slowdown_probability;
        } else if (line.find("acceleration_probability", 2) != string::npos) {
          csin >> car_config.acceleration_probability;
        } else if (line.find("max_speed", 2) != string::npos) {
          csin >> car_config.max_speed;
        } else if (line.find("min_speed", 2) != string::npos) {
          csin >> car_config.min_speed;
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

void Config::loadFromBinaryInteger(int binary_integer) {

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
  car_config.slowdown_probability = float(tmp)/100.0; 

  for (i = 14, tmp = 0; i >= 8; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<(i-8);
    }
  }
  car_config.acceleration_probability = float(tmp)/100.0; 

  for (i = 7, tmp = 0; i >= 4; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<(i-4);
    }
  }
  car_config.max_speed = tmp; 

  for (i = 3, tmp = 0; i >= 0; i--) {
    if (binary_integer>>i & 1) {
        tmp |= 1<<i;
    }
  }
  car_config.min_speed = tmp; 

  car_configs[0] = car_config; 
}
