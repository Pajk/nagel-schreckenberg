#include <cstdlib>
#include <sstream>
#include <iterator>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cstring>

#include "car_factory/csv_car_factory.h"
#include "car.h"
#include "config.h"

using namespace std;

CsvCarFactory * car_factory;
Config * config;

void loadCarFactory(char * data_file) {

  config = new Config();
  config->loadFromFile("data/default.config");
  config->setTrackLength(5349);

  car_factory = new CsvCarFactory(data_file, config);
  // cerr << "Data loaded from '" << data_file << "'.\n";
}

typedef struct {
  long cars_in;
  long cars_out;
  float mean_time;
  int cars_in_unknown;
  int cars_in_1;
  int cars_in_2;
  int cars_in_3;
  int cars_in_4;
  int cars_in_5;
} segment_info;

void extract(int minutes) {

  car_factory->sortByTimeIn();
  Car * car = car_factory->nextCar();

  map<time_t, segment_info> statistics;

  // zaokrouhli cas na nejblizsi cas delitelny 5 minutama
  time_t segment_start = car->getTimeIn();
  struct tm * timeinfo;
  timeinfo = localtime(&segment_start);
  timeinfo->tm_sec = 0;
  timeinfo->tm_min = timeinfo->tm_min - timeinfo->tm_min % minutes;
  segment_start = mktime(timeinfo);

  // delka jednoho segmentu
  time_t segment_length = minutes * 60;

  // prvni pruchod - zjisteni, kolik aut vjelo na silnici v aktualnim casovem useku
  while (car) {

    // kontrola, zda je auto jeste v aktualnim casovem useku, pokud ne, ulozi se
    // statistiky segmentu a zacne se zpracovavat dalsi
    // pocet aut, ktera opustila vozovku
    // pocet aut, ktera vjela na vozovku
    // prumerna doba dojezdu
    if (car->getTimeIn() > segment_start + segment_length) {
      int jump = (car->getTimeIn() - segment_start) / segment_length;
      segment_start += jump * segment_length;
    }

    statistics[segment_start].cars_in++;

    switch (car->getCarClass()) {
      case 1: statistics[segment_start].cars_in_1++; break;
      case 2: statistics[segment_start].cars_in_2++; break;
      case 3: statistics[segment_start].cars_in_3++; break;
      case 4: statistics[segment_start].cars_in_4++; break;
      case 5: statistics[segment_start].cars_in_5++; break;
      default: statistics[segment_start].cars_in_unknown++; break;
    }

    delete car;
    car = car_factory->nextCar();
  }

  // druhy pruchod - zjisteni, kolik aut z trate odjelo a jaka byla jejich prumerna
  // doba dojezdu v jednotlivych casovych usecich
  car_factory->sortByExpectedTimeOut();
  car = car_factory->nextCar();
  segment_start = mktime(timeinfo);
  while (car) {
    // time_t in = car->getTimeIn();
    // time_t out = car->getExpectedTimeOut();
    // // cout << car->getId() << " " << ctime(&in) << " " << ctime(&out) << endl;
    // cout << car->getId() << " "
    //      << car->getTimeIn() << " "
    //      << car->getExpectedTimeOut() << " "
    //      << car->getExpectedTime() << endl;
    // cout << ctime(&in);
    // cout << ctime(&out);
    // pokud auto opousti vozovku v case, ktery uz neni v aktualnim casovem useku,
    // spocita se prumerny cas dojezdu pro aktualni casovy usek a zacne se
    // zpracovavat dalsi casovy usek, ve kterem toto vozidlo je
    if (car->getTimeIn() + car->getExpectedTime() > segment_start + segment_length) {
      if (statistics[segment_start].cars_out > 0) {
        statistics[segment_start].mean_time /= statistics[segment_start].cars_out;
      }
      // skok na dalsi segment
      int jump = (car->getTimeIn() + car->getExpectedTime() - segment_start) / segment_length;
      for (int i = 0; i < jump; ++i) {
        segment_start += segment_length;
        statistics[segment_start].cars_out = 0;
      }
    }

    statistics[segment_start].cars_out++;
    statistics[segment_start].mean_time += car->getExpectedTime();

    delete car;
    car = car_factory->nextCar();
  }

  char buffer [80];

  // vytisknuti hlavicky
  cout << "\"Time\";\"TravelTime\";\"CarsIn\";\"CarsOut\";\"CarsIn1\";\"CarsIn2\";\"CarsIn3\";\"CarsIn4\";\"CarsIn5\";\"CarsInUnknown\";\n";

  for (map<time_t, segment_info>::iterator it = statistics.begin(); it != statistics.end(); ++it) {

    // tisk casu do stringu
    timeinfo = localtime(&(it->first));
    strftime(buffer, 80, "%d-%m-%Y %H:%M:%S", timeinfo);

    cout << '"' << buffer << "\";"
      << it->second.mean_time << ";"
      << it->second.cars_in << ";"
      << it->second.cars_out << ";"
      << it->second.cars_in_1 << ";"
      << it->second.cars_in_2 << ";"
      << it->second.cars_in_3 << ";"
      << it->second.cars_in_4 << ";"
      << it->second.cars_in_5 << ";"
      << it->second.cars_in_unknown << endl;
  }
}

void analyze() {

  map<int, vector<int> > statistics;
  int track_length = 5349;
  Car * car;

  while ((car = car_factory->nextCar()) != NULL) {
    statistics[car->getCarClass()].push_back(car->getExpectedTime());
  }

  // tisk hlavicky
  for (int i = 0; i < 57; ++i) cout << '-';
  cout << "\n| id\t| #\t| T[s]\t| speed[m/s]\t| speed[km/h]\t|" << endl;
  for (int i = 0; i < 57; ++i) cout << '-'; cout << endl;

  int sum, timelen;
  for (map<int,vector<int> >::iterator it = statistics.begin(); it != statistics.end(); ++it) {

    // tisk id tridy vozidla a poctu vozidel dane tridy v sade
    cout << "| " << it->first << "\t| "
         << it->second.size() << "\t| ";

    // vypocet a tisk prumerne doby dojezdu dane tridy
    sum = 0;
    for(vector<int>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
      sum += *vit;

    timelen = sum / it->second.size();
    cout << timelen << "\t| ";

    // tisk prumerne rychlosti v m/s a v km/h
    cout << float(track_length)/float(timelen) << "\t| "
         << float(track_length)/float(timelen)*3.6 << "\t|" << endl;
  }
  for (int i = 0; i < 57; ++i) cout << '-'; cout << endl;
}

int main(int argc, char **argv) {

  if (argc > 2) {

    loadCarFactory(argv[2]);

    if (strcmp(argv[1], "analyze") == 0) {
      analyze();
    } else if (strcmp(argv[1], "extract") == 0) {
      // pokud je nastavena delka intervalu [v minutach], pouzije se, jinak 5 minut
      if (argc >= 4) {
        extract(atoi(argv[3]));
      } else {
        extract(5);
      }
    } else {
      cerr << "Bad option, first argument has to be one of [analyze, convert].\n";
    }
  } else {
    cerr << "First argumet has to be one of [analyze, convert].\n";
  }

  delete car_factory;
  delete config ;
}
