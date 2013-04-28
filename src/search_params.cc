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

using namespace std;

// ============================ parametry evoluce =============================

// toto nastaveni je pretizeno hodnotami v souboru ga_params.txt
const int popsize = 100;      // velikost populace
const int ngens = 200;       // max pocet generaci
const float pmut = 0.3;       // pravdepodobnost mutace
const float pcross = 0.1;     // krizeni

CsvCarFactory *car_factory;

//#define DEBUG               // zapnuti ladicich vypisu

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

// =========================== fitness funkce =================================

float Fitness(GAGenome& g) {

    GA1DBinaryStringGenome &genome = (GA1DBinaryStringGenome &)g;

    // ziskani parametru ve forme integeru
    int params = 0;
    for (int i = 0; i < genome.length(); i++) {
        params |= genome.gene(i)<<i;
    }
    // cout << params << endl;
    Config *config = new Config();
    config->loadFromInteger(params);

    Statistics *statistics = new Statistics();
    car_factory->resetIterator();
    car_factory->setConfig(config);
    car_factory->setStatistics(statistics);

    int track_length = config->getNumberOfTrackCells();
    // int track_max_speed = config->getTrackMaxSpeed();
    Track *track = new Track(car_factory, track_length, 5);

    while (track->isLive()) {
        track->step();
    }

    float fitness = statistics->getMeanError();

    cout << "======================================================" << endl
         << "== FITNESS: " << fitness << endl;

    config->print();

    statistics->print();

    delete track;
    delete config;
    delete statistics;

    return fitness;
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

    GARandomSeed(time(NULL)*getpid());

    // genom - matice parametru modelu
    GA1DBinaryStringGenome genom(21, Fitness);

    car_factory = new CsvCarFactory("data/samples.csv");

    // nastaveni genetickych operatoru
    //genom.initializer(Initializer);
    //genom.mutator(Mutation);
    //genom.crossover(GA1DBinaryStringGenome::OnePointCrossover);

    // vytvoreni ga
    #if GA == 1
        GASimpleGA ga(genom);
    #elif GA == 2
        GASteadyStateGA ga(genom);
        ga.pReplacement(0.9);
    #elif GA == 3
        GADemeGA ga(genom);
    #endif

    ga.populationSize(popsize);
    ga.nGenerations(ngens);
    ga.pCrossover(pcross);
    ga.pMutation(pmut);

    ga.minimize();

    GATournamentSelector selector;
    ga.selector(selector);

    // moznost nastaveni parametru ze souboru a pres argumenty prikazove radky
    ga.parameters("data/ga_params.txt", gaTrue);
    ga.parameters(argc,argv);

    // start evolucniho procesu
    ga.initialize();
    while (!ga.done()) {

        ++ga;
//        #ifndef TEST
        if ((ga.statistics().generation() % ga.scoreFrequency()) == 0) {
            cout << "Generace " << ga.statistics().generation()
                 << " - Nejlepsi fitness: " << ga.population().min() << endl;
        }
//        #endif

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
    int params = 0;
    for (int i = 0; i < genome.length(); i++) {
        params |= genome.gene(i)<<i;
    }
    Config *config = new Config();
    config->loadFromInteger(params);
    config->print();


    return 0;
}
