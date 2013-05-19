#include <stdio.h>
#include <iostream>
#include <string>
#include <ga/ga.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>
#include <cstdlib>

#include "track.h"
#include "car.h"
#include "car_factory/simple_car_factory.h"
#include "car_factory/csv_car_factory.h"
#include "config.h"
#include "statistics.h"
#include "world.h"

using namespace std;

// ============================ parametry evoluce =============================

CsvCarFactory *car_factory;

// nazev souboru, do ktere se ulozi nejlepsi nalezene reseni:
#ifndef OUTPUT_FILE
    #define OUTPUT_FILE "results.txt"
#endif

/**
 * GA, ktery se ma pouzit
 * 1 - GASimpleGA
 * 2 - GASteadyStateGA
 * 3 - GADemeGA
 */
#ifndef GA
    #define GA 2
#endif

#ifndef CAR_TYPES
    #define CAR_TYPES 5
#endif

#ifndef DEFAULT_CAR
    #define DEFAULT_CAR 0
#endif

float Objective(GAGenome & g);
int MultiPointCrossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2);

//== Rizeni evoluce ============================================================

/** genetic operators for this class
 * GA1DBinaryStringGenome::UniformInitializer
 * GA1DBinaryStringGenome::SetInitializer
 * GA1DBinaryStringGenome::UnsetInitializer
 * GA1DBinaryStringGenome::FlipMutator
 * GA1DBinaryStringGenome::BitComparator
 * GA1DBinaryStringGenome::UniformCrossover
 * GA1DBinaryStringGenome::EvenOddCrossover
 * GA1DBinaryStringGenome::OnePointCrossover
 * GA1DBinaryStringGenome::TwoPointCrossover
 *
 * default genetic operators for this class
 * initialization: GA1DBinaryStringGenome::UniformInitializer
 * comparison: GA1DBinaryStringGenome::BitComparator
 * mutation: GA1DBinaryStringGenome::FlipMutator
 * crossover: GA1DBinaryStringGenome::OnePointCrossover
 */
int main(int argc, char **argv) {

    srand(time(NULL)*getpid());
    GARandomSeed(time(NULL)*getpid());

    /**
     * vytvoreni genomi - matice parametru modelu,
     * makra s poctem bitu jednotlivych parametru jsou definovana v config.h
     */
    GA1DBinaryStringGenome genome(BITS_TRACK + CAR_TYPES * BITS_CAR, Objective);

    if (argc >= 2) {
        car_factory = new CsvCarFactory(argv[1]);
        std::cout << "Train data loaded from '" << argv[1] << "'.\n";
    } else {
        car_factory = new CsvCarFactory("data/samples_train.csv");
    }

    // nastaveni genetickych operatoru
    //genom.mutator(Mutation);

    // nastaveni metody krizeni
    genome.crossover(MultiPointCrossover);

    // vytvoreni ga
    #if GA == 1
        GASimpleGA ga(genome);
    #elif GA == 2
        GASteadyStateGA ga(genome);
    #elif GA == 3
        GADemeGA ga(genome);
    #endif

    ga.minimize();

    GATournamentSelector selector;
    ga.selector(selector);

    // pouzijeme sigma truncation skalovani
    GASigmaTruncationScaling scaling;
    ga.scaling(scaling);

    // moznost nastaveni parametru ze souboru a pres argumenty prikazove radky
    ga.parameters("data/ga_params.txt", gaTrue);
    ga.parameters(argc, argv);

    // start evolucniho procesu
    // ga.initialize();
    // while (!ga.done()) {
    //     ++ga;
    //     // if ((ga.statistics().generation() % ga.scoreFrequency()) == 0) {
    //         cout << "Generace " << ga.statistics().generation()
    //              << " - Nejlepsi fitness: " << ga.population().max() << endl;

    //             GA1DBinaryStringGenome& g = (GA1DBinaryStringGenome&)ga.statistics().bestIndividual();

    //             Config *config = new Config();
    //             config->loadFromGABinaryString(g, DEFAULT_CAR);
    //             config->print();
    //     // }

    //     if (ga.population().max() == 100.0)
    //         break;
    // }

    ga.evolve();
    ga.flushScores();

    // tisk vysledku
    cout << ga.statistics() << endl;
    cout << "Nejlepsi reseni: " << ga.statistics().bestIndividual() << endl
         << "Fitness = " << ga.population().max() << endl;

    // tisk nejlepsiho jedince do souboru
    ofstream file(OUTPUT_FILE);
    file << ga.statistics().bestIndividual() << endl;

    // tisk nejlepsiho reseni
    GA1DBinaryStringGenome& g = (GA1DBinaryStringGenome&)ga.statistics().bestIndividual();

    Config *config = new Config();
    config->loadFromGABinaryString(g, DEFAULT_CAR);
    config->print();

    return 0;
}

//== Hodnotici funkce ==========================================================
float
Objective(GAGenome& g) {

    GA1DBinaryStringGenome &genome = (GA1DBinaryStringGenome &)g;

    // #ifdef DEBUG
    // cout << "======================================================" << endl;
    // #endif

    Config *config = new Config();
    config->loadFromGABinaryString(genome, DEFAULT_CAR);

    Statistics *statistics = new Statistics(true, true);
    car_factory->resetIterator();
    car_factory->setConfig(config);

    Track track(config, car_factory);
    World world(statistics, config);
    world.addTrack(&track);

    while (world.isLive()) {
        world.step();
    }

    statistics->summaryCalculate();
    Summary stats = statistics->getSummaryData();
    float objective = stats.mape;

    #ifdef DEBUG
    cout << "\n==================================================================\n";
    cout << "== OBJECTIVE: " << objective << endl;
    // config->print();
    statistics->summaryPrint();
    #endif

    // delete track;
    delete config;
    delete statistics;

    return objective;
}

//== Vicebodove krizeni ========================================================
int
MultiPointCrossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2) {

  GA1DBinaryStringGenome &mom=(GA1DBinaryStringGenome &)p1;
  GA1DBinaryStringGenome &dad=(GA1DBinaryStringGenome &)p2;
  GA1DBinaryStringGenome *sis = NULL, *bro = NULL;

  int n=0, pos = 0, len = 0;

  /**
   * hledaji se parametry pouze pro jeden typ vozidla
   * Body krizeni budou v techto mistech:
   * delka bunky, vozidla | max., min. rychlost | pravdepodobnost zrychleni, zpomaleni
   */
  if (CAR_TYPES == 1) {

    if(c1) {
      sis=(GA1DBinaryStringGenome *)c1;
      n++;
    }

    if(c2) {
      bro=(GA1DBinaryStringGenome *)c2;
      n++;
    }

    // kopie delky bunky a delky vozidla
    len = BITS_TRACK+BITS_CAR_LENGTH;
    if (c1) sis->copy(mom, pos, pos, len);
    if (c2) bro->copy(dad, pos, pos, len);
    pos += len;

    // kopie maximalni a minimalni rychlosti
    len = BITS_MAX_SPEED + BITS_MIN_SPEED;
    if (c1) sis->copy(dad, pos, pos, len);
    if (c2) bro->copy(mom, pos, pos, len);
    pos += len;

    // kopie pravdepodobnosti zrychleni a zpomaleni
    len = BITS_SLOWDOWN_P + BITS_ACC_P;
    if (c1) sis->copy(mom, pos, pos, len);
    if (c2) bro->copy(dad, pos, pos, len);

  }

  /**
   * Hledaji se parametry pro vice typu vozidla
   * Body krizeni budou v techto mistech:
   * delka jedne bunky | nastaveni vozidla 1 | nastaveni vozidla 2 | ...
   */
  else {

    if(c1) {
      sis=(GA1DBinaryStringGenome *)c1;
      n++;
    }

    if(c2) {
      bro=(GA1DBinaryStringGenome *)c2;
      n++;
    }

    // kopie delky bunky
    len = BITS_TRACK;
    if (c1) sis->copy(mom, pos, pos, len);
    if (c2) bro->copy(dad, pos, pos, len);
    pos += len;

    for (int i = 0; i < CAR_TYPES; ++i) {
      len = BITS_CAR;
      if (c1) sis->copy((i%2 == 0 ? dad : mom), pos, pos, len);
      if (c2) bro->copy((i%2 == 0 ? mom : dad), pos, pos, len);
      pos += len;
    }
  }

  return n;
}
