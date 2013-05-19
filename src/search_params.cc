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

// toto nastaveni je pretizeno hodnotami v souboru ga_params.txt
const int popsize = 100;      // velikost populace
const int ngens = 200;       // max pocet generaci
const float pmut = 0.3;       // pravdepodobnost mutace
const float pcross = 0.1;     // krizeni

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
    #define CAR_TYPES 6
#endif

#ifndef DEFAULT_CAR
    #define DEFAULT_CAR 0
#endif

// =========================== fitness funkce =================================

float Fitness(GAGenome& g) {

    GA1DBinaryStringGenome &genome = (GA1DBinaryStringGenome &)g;

    // #ifdef DEBUG
    // cout << "======================================================" << endl;
    // #endif

    Config *config = new Config();
    config->loadFromGABinaryString(genome, DEFAULT_CAR);

    // #ifdef DEBUG
    // cout << endl;
    // config->print();
    // #endif

    Statistics *statistics = new Statistics();
    car_factory->resetIterator();
    car_factory->setConfig(config);

    Track *track = new Track(config, car_factory);
    World world(statistics, config);
    world.addTrack(track);

    while (track->isLive()) {
        track->step();
    }

    // float fitness = statistics->getMeanError();
    float fitness = fabs(statistics->getMeanTravelTime() - statistics->getMeanExpectedTravelTime());

    #ifdef DEBUG
    cout << "== FITNESS: " << fitness << endl;
    // statistics->print();
    #endif

    delete track;
    delete config;
    delete statistics;

    return fitness;
}

int
SinglePointCrossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2){
  GA1DBinaryStringGenome &mom=(GA1DBinaryStringGenome &)p1;
  GA1DBinaryStringGenome &dad=(GA1DBinaryStringGenome &)p2;

  int n=0;
  unsigned int site = 4;
  unsigned int len = mom.length() - site;

  if(c1){
    GA1DBinaryStringGenome &sis=(GA1DBinaryStringGenome &)*c1;
    sis.copy(mom, 0, 0, site);
    sis.copy(dad, site, site, len);
    n++;
  }
  if(c2){
    GA1DBinaryStringGenome &bro=(GA1DBinaryStringGenome &)*c2;
    bro.copy(dad, 0, 0, site);
    bro.copy(mom, site, site, len);
    n++;
  }

  return n;
}


// ============================ rizeni evoluce ==============================

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

    // genom - matice parametru modelu, 4  bity pro nastaveni delky trate
    // 26 bitu pro nastaveni kazdeho typu vozidla
    GA1DBinaryStringGenome genom(BITS_TRACK + CAR_TYPES * BITS_CAR, Fitness);

    if (argc >= 2) {
        car_factory = new CsvCarFactory(argv[1]);
        std::cout << "Train data loaded from '" << argv[1] << "'.\n";
    } else {
        car_factory = new CsvCarFactory("data/samples_train.csv");
    }

    // nastaveni genetickych operatoru
    //genom.initializer(Initializer);
    //genom.mutator(Mutation);
    //genom.crossover(SinglePointCrossover);

    // vytvoreni ga
    #if GA == 1
        GASimpleGA ga(genom);
    #elif GA == 2
        GASteadyStateGA ga(genom);
        // ga.pReplacement(0.9);
    #elif GA == 3
        GADemeGA ga(genom);
    #endif

    // ga.populationSize(popsize);
    // ga.nGenerations(ngens);
    // ga.pCrossover(pcross);
    // ga.pMutation(pmut);

    ga.minimize();

    // GATournamentSelector selector;
    // ga.selector(selector);

    // moznost nastaveni parametru ze souboru a pres argumenty prikazove radky
    ga.parameters("data/ga_params.txt", gaTrue);
    ga.parameters(argc,argv);

    // start evolucniho procesu
    ga.initialize();
    while (!ga.done()) {

        ++ga;
        if ((ga.statistics().generation() % ga.scoreFrequency()) == 0) {
            cout << "Generace " << ga.statistics().generation()
                 << " - Nejlepsi fitness: " << ga.population().min() << endl;

                GA1DBinaryStringGenome& genome = (GA1DBinaryStringGenome&)ga.statistics().bestIndividual();

                Config *config = new Config();
                config->loadFromGABinaryString(genome, DEFAULT_CAR);
                config->print();
        }

        if (ga.population().min() == 0)
            break;
    }
    ga.flushScores();

    #ifdef TEST
        cout << ga.generation() << " " << ga.statistics().bestIndividual().score() << endl;
    #else
        // tisk statistik
        cout << endl << "Statistika:" << endl;
        cout << ga.statistics() << endl;

        cout << "Nejlepsi reseni: " << ga.statistics().bestIndividual() <<
                ", Fitness = " << ga.population().min() << endl;

        // tisk nejlepsiho jedince do souboru
        ofstream file(OUTPUT_FILE);
        file << ga.statistics().bestIndividual() << endl;
    #endif

    // tisk nejlepsiho reseni
    GA1DBinaryStringGenome& genome = (GA1DBinaryStringGenome&)ga.statistics().bestIndividual();

    Config *config = new Config();
    config->loadFromGABinaryString(genome, DEFAULT_CAR);
    config->print();

    return 0;
}
