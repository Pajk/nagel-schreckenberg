#include <cstdlib>
#include <iostream>
#include <ctime>

#include "world.h"
#include "car.h"
#include "car_factory/car_factory.h"
#include "cell.h"
#include "statistics.h"
#include "track.h"
#include "config.h"

World::World(Statistics * statistics, Config * config) {

  this->statistics = statistics;
  this->config = config;

  this->track = NULL;
  sim_time = 0;
  delete_list = NULL;
}

World::~World() {}

void World::calculateSpeeds() {
  if (track != NULL) {
    track->calculateSpeeds();
  }
}

void World::moveCars() {
  // vykonani kroku simulace
  if (track == NULL) return;

  track->moveCars();

  // smazani aut, ktera opustila vozovku
  if (delete_list) {
    // zalogovani casu dojezdu a rychlosti
    Car * tmp_car = delete_list;
    while (tmp_car != NULL) {
        logStats(tmp_car);
        tmp_car = tmp_car->getCarInFront();
    }
    // Smazani vozidel, ktera vyjela z vozovky
    delete delete_list;
    delete_list = NULL;
  }

  // inkrementace casu simulace
  sim_time++;

  // v kazdem kroku se kontroluje jedna bunka vozovky (v polovine trasy)
  // a inkrementuje se citac, pokud je obsazena
  statistics->logCellOccupancy(track->getMiddleCell()->isOccupied());

  // vypocet toku a hustoty pro ubehly interval o nastavene delce
  if (sim_time%(config->getStatsFrequency()) == 0) {
    statistics->calculatePrintAndReset(sim_time);
  }
}

/**
 * Krok simulace
 */
void World::step() {
  calculateSpeeds();
  moveCars();
}

bool World::isLive() {

  if (config->getMaxTime() > 0 && config->getMaxTime() < sim_time) {
    return false;
  }

  if (track) {
    return track->isLive();
  } else {
    return false;
  }
}

void World::addOutOfTrackCar(Car * car) {

  // pokud seznam neni prazdny, dojde k pripojeni daneho vozidla
  if (delete_list) {
    delete_list->setCarBehind(car);
  }
  // prvni vozidlo z fronty pro smazani se nastavi jako predchozi vozidlo
  // daneho vozidla, tyto vazby je nutne dodrzovat, aby bylo umozneno
  // jednoduche smazani celeho seznamu
  car->setCarInFront(delete_list);
  delete_list = car;

  // vynulovani reference na vozidlo, ktere bylo pravne pridano do seznamu ke smazani
  if (car->getCarBehind()) {
    car->getCarBehind()->setCarInFront(NULL);
  }

  car->setCell(NULL);

}

void World::logStats(Car * car) {
  statistics->logCarTime(sim_time, car);
}

void World::logMeanSpeed(float mean_speed) {
  statistics->logMeanSpeed(mean_speed);
}

void World::logOccupancy(float occupancy) {
  statistics->logOccupancy(occupancy);
}

void World::logDensity(float density) {
  statistics->logDensity(density);
}
