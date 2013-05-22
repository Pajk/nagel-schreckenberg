#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cmath>

#include "ga_config.h"

using namespace std;

#define STR(s) #s
#define GA_LOAD_CONFIG(opt) \
    if (line.find(STR(opt =)) != string::npos) { \
        sin >> opt; \
    } else

void GAConfig::loadFromFile(const char *filename) {

  ifstream fin(filename);

  if (!fin.good()) {
    cout << "Cannot open config file '" << filename << "'." << endl;
    exit(-1);
  }

  string line;
  size_t first_char;

  while(getline(fin, line)) {
    istringstream sin(line.substr(line.find("=") + 1));

    if (line.empty()) continue;

    // kontrola zda nejde o komentar
    first_char = line.find_first_not_of(" \t");
    if (first_char != string::npos && line.at(first_char) == '#') {
      continue;
    }

    GA_LOAD_CONFIG(ga)
    GA_LOAD_CONFIG(population_size)
    GA_LOAD_CONFIG(mutation_probability)
    GA_LOAD_CONFIG(crossover_probability)
    GA_LOAD_CONFIG(number_of_generations)
    GA_LOAD_CONFIG(score_filename)
    GA_LOAD_CONFIG(score_frequency)
    GA_LOAD_CONFIG(flush_frequency)
    GA_LOAD_CONFIG(replacement_percentage)
    GA_LOAD_CONFIG(replacement_number)
    GA_LOAD_CONFIG(number_of_populations)
    GA_LOAD_CONFIG(migration_number)
    GA_LOAD_CONFIG(terminator)
    GA_LOAD_CONFIG(generations_to_convergence)
    GA_LOAD_CONFIG(convergence_percentage)
    GA_LOAD_CONFIG(number_of_offspring)
    GA_LOAD_CONFIG(scaling)
    GA_LOAD_CONFIG(sigma_truncation_multiplier)
    GA_LOAD_CONFIG(power_scaling_factor)
    GA_LOAD_CONFIG(linear_scaling_multiplier)

    GA_LOAD_CONFIG(car_types)
    GA_LOAD_CONFIG(default_car)
    GA_LOAD_CONFIG(train_file)
    GA_LOAD_CONFIG(test_file)
    GA_LOAD_CONFIG(slow_to_start_probability)
    GA_LOAD_CONFIG(slow_to_stop)
    GA_LOAD_CONFIG(stats_frequency)

    GA_LOAD_CONFIG(track_length)
    GA_LOAD_CONFIG(periodic_boundary)
    GA_LOAD_CONFIG(car_factory)
    GA_LOAD_CONFIG(scf_interval)
    GA_LOAD_CONFIG(ncf_deviation)
    GA_LOAD_CONFIG(ncf_mean)
    GA_LOAD_CONFIG(max_time)
    GA_LOAD_CONFIG(true_slowdown)

    GA_LOAD_CONFIG(objective_value)
    GA_LOAD_CONFIG(minimize)
    {
      // ..
    }

  }
  stats_frequency *= 60;
  max_time *= 3600;
}

void GAConfig::print() {
    cout << "== GA config ==\n"
         << "ga = " << ga << endl
         << "replacement_percentage = " << replacement_percentage << endl
         << "replacement_number = " << replacement_number << endl
         << "number_of_populations = " << number_of_populations << endl
         << "migration_number = " << migration_number << endl
         << "number_of_offspring = " << number_of_offspring << endl
         << "population_size = " << population_size << endl
         << "mutation_probability = " << mutation_probability << endl
         << "crossover_probability = " << crossover_probability << endl;

    if (score_filename != NULL) {
      cout << "score_filename = " << score_filename << endl;
    }

    cout << "score_frequency = " << score_frequency << endl
         << "flush_frequency = " << flush_frequency << endl
         << "terminator = " << terminator << endl
         << "number_of_generations = " << number_of_generations << endl
         << "generations_to_convergence = " << generations_to_convergence << endl
         << "convergence_percentage = " << convergence_percentage << endl
         << "scaling = " << scaling << endl
         << "linear_scaling_multiplier = " << linear_scaling_multiplier << endl
         << "sigma_truncation_multiplier = " << sigma_truncation_multiplier << endl
         << "power_scaling_factor = " << power_scaling_factor << endl
         << "car_types = " << car_types << endl
         << "default_car = " << default_car << endl;
    if (train_file != NULL) {
      cout << "train_file = " << train_file << endl;
    }
    if (test_file != NULL) {
      cout << "test_file = " << test_file << endl;
    }
    cout << "track_length = " << track_length << endl
         << "slow_to_stop = " << slow_to_stop << endl
         << "slow_to_start_probability = " << slow_to_start_probability << endl
         << "stats_frequency = " << stats_frequency/60 << endl
         << "car_factory = " << car_factory << endl
         << "scf_interval = " << scf_interval << endl
         << "ncf_mean = " << ncf_mean << endl
         << "ncf_deviation = " << ncf_deviation << endl
         << "periodic_boundary = " << periodic_boundary << endl
         << "max_time = " << max_time/3600 << endl
         << "objective_value = " << objective_value << endl
         << "minimize = " << minimize << endl
         << "true_slowdown = " << true_slowdown << endl << endl;
}
