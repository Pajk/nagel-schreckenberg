#ifndef _GA_CONFIG_H
#define _GA_CONFIG_H

#include <cstring>
#include <iostream>

#define GA_CONFIG_ITEM(type, name, canonical) \
  protected: type canonical; \
  public: type get##name() { return canonical; }

using std::string;

class GAConfig {

  GA_CONFIG_ITEM(int,    GA,                     ga)
  GA_CONFIG_ITEM(int,    PopulationSize,         population_size)
  GA_CONFIG_ITEM(float,  MutationProbability,    mutation_probability)
  GA_CONFIG_ITEM(float,  CrossoverProbability,   crossover_probability)
  GA_CONFIG_ITEM(int,    NumberOfGenerations,    number_of_generations)
  GA_CONFIG_ITEM(char *, ScoreFilename,          score_filename)
  GA_CONFIG_ITEM(int,    ScoreFrequency,         score_frequency)
  GA_CONFIG_ITEM(int,    FlushFrequency,         flush_frequency)
  GA_CONFIG_ITEM(float,  ReplacementPercentage,  replacement_percentage)
  GA_CONFIG_ITEM(int,    ReplacementNumber,      replacement_number)
  GA_CONFIG_ITEM(int,    NumberOfPopulations,    number_of_populations)
  GA_CONFIG_ITEM(int,    MigrationNumber,        migration_number)
  GA_CONFIG_ITEM(int,    Terminator,             terminator)
  GA_CONFIG_ITEM(int,    GenerationsToConvergence, generations_to_convergence)
  GA_CONFIG_ITEM(float,  ConvergencePercentage,  convergence_percentage)
  GA_CONFIG_ITEM(int,    NumberOfOffspring,      number_of_offspring)
  GA_CONFIG_ITEM(int,    Scaling,                scaling)
  GA_CONFIG_ITEM(float,  LinearScalingMultiplier, linear_scaling_multiplier)
  GA_CONFIG_ITEM(float,  SigmaTruncationMultiplier, sigma_truncation_multiplier)
  GA_CONFIG_ITEM(float,  PowerScalingFactor,     power_scaling_factor)

  GA_CONFIG_ITEM(int,    CarTypes,               car_types)
  GA_CONFIG_ITEM(int,    DefaultCar,             default_car)
  GA_CONFIG_ITEM(char *, TrainFile,              train_file)
  GA_CONFIG_ITEM(char *, TestFile,               test_file)
  GA_CONFIG_ITEM(bool,   SlowToStop,             slow_to_stop)
  GA_CONFIG_ITEM(float,  SlowToStartProbability, slow_to_start_probability)
  GA_CONFIG_ITEM(int,    StatsFrequency,         stats_frequency)

  GA_CONFIG_ITEM(int,     TrackLength,           track_length)
  GA_CONFIG_ITEM(int,     CarFactory,            car_factory)
  GA_CONFIG_ITEM(bool,    PeriodicBoundary,      periodic_boundary)
  GA_CONFIG_ITEM(int,     SCFInterval,           scf_interval)
  GA_CONFIG_ITEM(float,   NCFDeviation,          ncf_deviation)
  GA_CONFIG_ITEM(int,     NCFMean,               ncf_mean)
  GA_CONFIG_ITEM(unsigned long, MaxTime,         max_time)
  GA_CONFIG_ITEM(bool,    TrueSlowdown,          true_slowdown)

  GA_CONFIG_ITEM(bool,    Minimize,               minimize)
  GA_CONFIG_ITEM(int,     ObjectiveValue,         objective_value)

  public:
    GAConfig() {

      score_filename = new char[255];
      train_file = new char[255];
      test_file = new char[255];

      // vychozi hodnoty
      ga = 2;
      terminator = 1;
      scaling = 3;
      number_of_generations = 250;
      convergence_percentage = 0.99;
      generations_to_convergence = 20;
      crossover_probability = 0.9;
      mutation_probability = 0.01;
      population_size = 30;
      number_of_populations = 10;
      replacement_percentage = 0.25;
      replacement_number = 5;
      score_frequency = 1;
      flush_frequency = 0;
      strcpy(score_filename, "generations.dat");
      number_of_offspring = 2;
      migration_number = 5;
      linear_scaling_multiplier = 1.2;
      sigma_truncation_multiplier = 2.0;
      power_scaling_factor = 1.0005;
      strcpy(train_file, "data/evropska_20.csv");
      strcpy(test_file, "data/evropska_21.csv");
      stats_frequency = 0;
      objective_value = 1;
      minimize = true;
      true_slowdown = false;
    }

    ~GAConfig() {
      if (score_filename != NULL) {
        delete[] score_filename;
      }
      if (train_file != NULL) {
        delete[] train_file;
      }
      if (test_file != NULL) {
        delete[] test_file;
      }
    }

    void loadFromFile(const char *filename);

    void print();
};

#endif
