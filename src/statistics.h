#ifndef _STATISTICS_H
#define _STATISTICS_H

#include <vector>
#include <cstdlib>

class Car;

struct CarTime {
  int total_time;
  int expected_time;
  int car_id;
  long left_at;
};

class Statistics {

  std::vector<CarTime> car_times;

  float mae;
  float mape;
  float rmse;
  float flow;
  float density;
  float faster_mean_error;
  float slower_mean_error;
  int min_error;
  int max_error;
  int slower_cars;
  int faster_cars;
  float mean_travel_time;
  float mean_expected_travel_time;

  long time_from;
  long time_to;
  // pocet kroku simulace, ve kterem byla sledovana bunka obsazena
  long cell_time_occupied;
  float mean_speed;

  /**
   * nastaveni zda se maji statistiky tisknout ve forme tabulky,
   * data v tomto formatu mohou byt pouzita pro vykresleni grafu gnuplotem
   * nebo primo v latexu pomoci pgfplots rozsireni
   */
  bool table_format;

  public:

    Statistics(bool table_format = false);

    /**
     * Vynulovani vsech statistik a smazani nasbiranych dat
     */
    void reset(long time_from = 0);

    /**
     * Ulozeni casu dojezdu jednoho vozidla
     */
    void logCarTime(long current_time, Car * car);

    /**
     * Vypocet statistik s nasbiranych dat
     */
    void calculate(long time_to = 0);

    /**
     * Tisk statistik
     */
    void print();

    void calculatePrintAndReset(long current_time = 0) {
      calculate(current_time);
      print();
      reset(current_time);
    }

    void logCellOccupancy(bool occupied);
    void logMeanSpeed(float mean_speed);

    float getMeanAbsoluteError() { return mae; }
    float getMeanAbsolutePercentageError() { return mape; }
    float getRootMeanSquareError() { return rmse; }
    float getSlowerMeanError() { return slower_mean_error; }
    float getFasterMeanError() { return faster_mean_error; }
    float getMeanTravelTime() { return mean_travel_time; }
    float getMeanExpectedTravelTime() { return mean_expected_travel_time; }
    float getFlow() { return flow; }
    float getDensity() { return density; }
    float getMeanSpeed() { return mean_speed; }
    int getMaxError() { return max_error; }
    int getMinError() { return min_error; }
    int getSlowerCars() { return slower_cars; }
    int getFasterCars() { return faster_cars; }
};

#endif

