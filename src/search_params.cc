/*
 * Datum:   20.5.2013
 * Autor:   Pavel Pokorny, xpokor12@stud.fit.vutbr.cz
 * Projekt: DP - Kalibrace mikrosimulacniho modelu dopravy
 */

#include <stdio.h>
#include <iostream>
#include <string>
#include <ga/ga.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>
#include <cstdlib>
#include <csignal>

#include "track.h"
#include "car.h"
#include "car_factory/simple_car_factory.h"
#include "car_factory/normal_car_factory.h"
#include "car_factory/prague_car_factory.h"
#include "config.h"
#include "ga_config.h"
#include "statistics.h"
#include "world.h"

using namespace std;

// ============================ parametry evoluce =============================

CarFactory * car_factory;
GAConfig * ga_config;
bool stop_evolution = false;

// nazev souboru, do ktere se ulozi nejlepsi nalezene reseni:
#define OUTPUT_FILE "results.txt"
#define GA_CONFIG_FILE "configs/default.ga_config"

float Objective(GAGenome & g);
int MultiPointCrossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2);
void stopEvolution(int s);
CarFactory * getCarFactory();
void initConfig(Config * config);

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

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = stopEvolution;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    srand(time(NULL)*getpid());
    GARandomSeed(time(NULL)*getpid());

    ga_config = new GAConfig();
    if (argc >= 2) {
        ga_config->loadFromFile(argv[1]);
    } else {
        ga_config->loadFromFile(GA_CONFIG_FILE);
    }
    ga_config->print();

    car_factory = getCarFactory();

    /**
     * vytvoreni genomu - zakodovane parametry modelu,
     * makra s poctem bitu jednotlivych parametru jsou definovana v config.h
     */
    GA1DBinaryStringGenome genome(BITS_TRACK + ga_config->getCarTypes() * BITS_CAR, Objective);

    // nastaveni genetickych operatoru
    //genom.mutator(Mutation);

    // nastaveni metody krizeni
    genome.crossover(MultiPointCrossover);

    // vytvoreni ga
    GAGeneticAlgorithm * ga_ptr = NULL;
    switch(ga_config->getGA()) {
      case 1:
        ga_ptr = new GASimpleGA(genome);
        break;
      case 2:
        ga_ptr = new GASteadyStateGA(genome);
        ((GASteadyStateGA*)ga_ptr)->pReplacement(ga_config->getReplacementPercentage());
        break;
      case 3:
        ga_ptr = new GADemeGA(genome);
        ((GADemeGA*)ga_ptr)->nReplacement(GADemeGA::ALL, ga_config->getReplacementNumber());
        ((GADemeGA*)ga_ptr)->nPopulations(ga_config->getNumberOfPopulations());
        ((GADemeGA*)ga_ptr)->nMigration(ga_config->getMigrationNumber());
      case 4:
        ga_ptr = new GAIncrementalGA(genome);
        ((GAIncrementalGA*)ga_ptr)->nOffspring(ga_config->getNumberOfOffspring());
        break;
    }
    GAGeneticAlgorithm &ga = *ga_ptr;

    // nastaveni parametru GA
    ga.populationSize(ga_config->getPopulationSize());
    ga.pMutation(ga_config->getMutationProbability());
    ga.pCrossover(ga_config->getCrossoverProbability());
    ga.scoreFilename(ga_config->getScoreFilename());
    ga.scoreFrequency(ga_config->getScoreFrequency());
    ga.flushFrequency(ga_config->getFlushFrequency());

    // budeme minimalizovat
    if (ga_config->getMinimize()) {
      ga.minimize();
    }

    GATournamentSelector selector;
    ga.selector(selector);

    // nastaveni skalovani
    switch (ga_config->getScaling()) {
      case 1: {
        GANoScaling scaling;
        ga.scaling(scaling);
        break;
      }
      case 2: {
        GALinearScaling scaling(ga_config->getLinearScalingMultiplier());
        ga.scaling(scaling);
        break;
      }
      case 3: {
        GASigmaTruncationScaling scaling(ga_config->getSigmaTruncationMultiplier());
        ga.scaling(scaling);
        break;
      }
      case 4: {
        GAPowerLawScaling scaling(ga_config->getPowerScalingFactor());
        ga.scaling(scaling);
      }
    }

    switch (ga_config->getTerminator()) {
      case 2:
        ga.terminator(GAGeneticAlgorithm::TerminateUponConvergence);
        ga.pConvergence(ga_config->getConvergencePercentage());
        ga.nConvergence(ga_config->getGenerationsToConvergence());
        break;
      case 3:
        ga.terminator(GAGeneticAlgorithm::TerminateUponPopConvergence);
        ga.pConvergence(ga_config->getConvergencePercentage());
        break;
      case 1:
      default:
        ga.terminator(GAGeneticAlgorithm::TerminateUponGeneration);
        ga.nGenerations(ga_config->getNumberOfGenerations());
        break;
    }

    // moznost nastaveni parametru pres argumenty prikazove radky
    ga.parameters(argc, argv);

    // start evolucniho procesu
    cout << "== Initial population\n";
    Statistics::printHeader();
    ga.initialize();
    while (!ga.done() && !stop_evolution) {

        cout << "== Generation " << ga.statistics().generation() << endl;

        Statistics::printHeader();

        // cout << flush;

        ga.step();

        cout << "Best individual score: "
             << (ga_config->getMinimize() ? ga.population().min() : ga.population().max()) << endl;

        // nalezeno nejlepsi mozne reseni
        if (ga_config->getMinimize()) {
          if (ga.population().min() == 0) break;
        }
    }

    // ga.evolve();
    ga.flushScores();

    // tisk vysledku
    cout << ga.statistics() << endl;
    cout << "Best individual: " << ga.statistics().bestIndividual() << endl
         << "Score = " << (ga_config->getMinimize() ? ga.population().min() : ga.population().max()) << endl;

    // tisk nejlepsiho jedince do souboru
    ofstream file(OUTPUT_FILE);
    file << ga.statistics().bestIndividual() << endl;

    // tisk nejlepsiho reseni
    GA1DBinaryStringGenome& best = (GA1DBinaryStringGenome&)ga.statistics().bestIndividual();

    Config config;
    initConfig(&config);
    config.loadFromGABinaryString(best, ga_config->getDefaultCar());
    config.print();

    // spusteni na testovaci sade, pokud jsou pozita data z prahy
    if (ga_config->getCarFactory() == 3) {

      Statistics::printHeader();

      delete car_factory;
      car_factory = new PragueCarFactory(ga_config->getTestFile(), &config);
      cout << ga_config->getTestFile() << " score: " << Objective(best) << endl;
      delete car_factory;

      car_factory = new PragueCarFactory("data/prague/evropska_20.csv", &config);
      cout << "data/prague/evropska_20.csv score: " << Objective(best) << endl;
      delete car_factory;

      car_factory = new PragueCarFactory("data/prague/evropska_21.csv", &config);
      cout << "data/prague/evropska_21.csv score: " << Objective(best) << endl;
      delete car_factory;

      car_factory = new PragueCarFactory("data/prague/evropska_22.csv", &config);
      cout << "data/prague/evropska_22.csv score: " << Objective(best) << endl;
    }

    delete ga_config;
    delete ga_ptr;
    delete car_factory;

    return 0;
}

//== Hodnotici funkce ==========================================================
float
Objective(GAGenome& g) {

    GA1DBinaryStringGenome &genome = (GA1DBinaryStringGenome &)g;

    // vytvoreni konfigurace simulace z kandidatniho reseni a z globalniho
    // nastaveni ktere je pro vsechny jedince stejne
    Config sim_config;
    initConfig(&sim_config);
    sim_config.loadFromGABinaryString(genome, ga_config->getDefaultCar());

    car_factory->setConfig(&sim_config);

    Statistics *statistics = new Statistics(&sim_config, true);

    Track track(&sim_config, car_factory);
    World world(statistics, &sim_config);
    world.addTrack(&track);

    while (world.isLive()) {
        world.step();
    }

    statistics->summaryCalculate();
    Summary stats = statistics->getSummaryData();

    float score = 0;
    switch (ga_config->getObjectiveValue()) {
      case 1: score = stats.mae; break;
      case 2: score = stats.mape; break;
      case 3: score = stats.rmse; break;
      case 4: score = stats.flow; break;
      case 5: score = stats.mean_speed; break;
      case 6: score = stats.cars; break;
      case 7: score = stats.mean_travel_time; break;
      default:
        cerr << "You have to set which objective value to use.\n";
        exit(-1);
    }

    // #ifdef DEBUG
    // cout << "\n========================================================================\n"
         // << " SCORE: " << score << endl;
    // sim_config.print();
    statistics->summaryPrint();
    // #endif

    delete statistics;

    return score;
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
  if (ga_config->getCarTypes() == 1) {

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

    for (int i = 0; i < ga_config->getCarTypes(); ++i) {
      len = BITS_CAR;
      if (c1) sis->copy((i%2 == 0 ? dad : mom), pos, pos, len);
      if (c2) bro->copy((i%2 == 0 ? mom : dad), pos, pos, len);
      pos += len;
    }
  }

  return n;
}

void stopEvolution(int s) {
  if (stop_evolution) {
    exit(-1);
  } else {
    stop_evolution = true;
    cout << "\nGoing to try best found solution on the test dataset.\n";
  }
}

CarFactory * getCarFactory() {

  switch(ga_config->getCarFactory()) {
    case 1:
      return new SimpleCarFactory(NULL);
    case 2:
      return new NormalCarFactory(NULL);
    case 3:
    default:
      cout << "Train data loaded from '" << ga_config->getTrainFile() << "'.\n";
      return new PragueCarFactory(ga_config->getTrainFile(), NULL);
  }
}

void initConfig(Config * config) {
  config->setSlowToStop(ga_config->getSlowToStop());
  config->setSlowToStartProbability(ga_config->getSlowToStartProbability());
  config->setStatsFrequency(ga_config->getStatsFrequency());
  config->setTrackLength(ga_config->getTrackLength());
  config->setPeriodicBoundary(ga_config->getPeriodicBoundary());
  config->setSCFInterval(ga_config->getSCFInterval());
  config->setNCFDeviation(ga_config->getNCFDeviation());
  config->setNCFMean(ga_config->getNCFMean());
  config->setMaxTime(ga_config->getMaxTime());
  config->setCarFactory(ga_config->getCarFactory());
  config->setTrueSlowdown(ga_config->getTrueSlowdown());
  config->setTableFormat(true);
}
