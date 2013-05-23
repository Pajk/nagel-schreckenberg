#ifndef _WORLD_H
#define _WORLD_H

#include "track.h"

class Car;
class CarFactory;
class Cell;
class Statistics;
class Config;
struct tm;

class World {

  // aktualni cas simulace
  unsigned long long sim_time;

  // vozidla, ktera vyjela z vozovky
  Car * delete_list;

  // globalni statistiky simulace
  Statistics * statistics;

  // modelovana vozovka
  Track * track;

  Config * config;

  public:

    World(Statistics * statistics, Config * config);
    ~World();

    // vrati aktualni cas simulace
    unsigned long long getCurrentTime() { return sim_time; }

    /**
     * pridani trate, na ktere bude probihat simulace
     * protoze simulujeme dopravu pouze na jedne trati a jednom pruhu,
     * obsahuje svet vzdy jednu cestu, pokud by mel byt simulator rozsiren,
     * muselo by byt mozne pridat libovolny pocet cest a nejakym zpusobem nadefinovat
     * jejich vzajemne propojeni
     */
    void addTrack(Track * track) { track->setWorld(this); this->track = track; }

    /**
     * Vykona jeden krok simulace, dojde k oznameni vsem
     * objektum reprezentujici cesty, aby se provedl jeden krok simulace,
     * dojde ke smazani objektu vozidel, ktera opustila vozovku a nakonec dojde
     * k inkrementaci casu simulace.
     */
    void step();
    void calculateSpeeds();
    void moveCars();

    /**
     * Predikat urcujici, zda ma simulace jeste pokracovat. Pokud uz na silnicich
     * nejsou zadna vozidla a nejsou ani cekajici na prihodny cas (tzn. generatory
     * vozidel uz negeneruji dalsi vozidla) vraci hodnotu false, v opacnem pripade
     * true.
     */
    bool isLive();

    /**
     * Pridani vozidla do seznamu vozidel urcenych ke smazani, jedna se o vozidla,
     * ktera opustila modelovanou dopravni sit. Tyto objekty by se mohly znovu
     * vyuzivat pri generovani vozidel, momentalne ale dochazi pouze k zapsani
     * jejich casu odjezdu do statistik a jejich smazani.
     */
    void addOutOfTrackCar(Car * car);

    /**
     * Vrati vozovku na ktere je simulovana doprava,
     * pokud by mel byt model rozsiren, aby modeloval dopravu na vice
     * vozovkach, metoda getTracks by vracela vektor vsech trati.
     */
    Track * getTrack() { return track; }

    /**
     * Zalogovani vyjezdu vozidla z trate, uklada se do statistik
     * pomoci objektu Statistics. Pokud by se modelovalo vice
     * trati, ukladaly by se hodnoty spolu s informaci z jake
     * vozovky vozidlo vyjelo.
     */
    void logStats(Car * car);

    /**
     * Zalogovani prumerne rychlosti vsech vozidel na trati v aktualnim
     * kroku simulace
     */
    void logMeanSpeed(float mean_speed);
    void logOccupancy(float occupancy);
    void logDensity(float density);
};

#endif
