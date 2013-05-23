#ifndef _STATISTICS_H
#define _STATISTICS_H

#include <vector>
#include <cstdlib>

class Car;
class Config;

struct CarTime {
  int total_time;
  int expected_time;
  int car_id;
  long left_at;
};

struct Summary {
  long t_from;
  long t_to;
  float mae;
  float mape;
  float rmse;
  float flow;
  float density;
  float mean_speed;
  int cars;
  int min_error;
  int max_error;
  int faster_cars;
  int slower_cars;
  float faster_mae;
  float slower_mae;
  float mean_travel_time;
  float mean_expected_travel_time;
  float occupancy;
};

class Statistics {

  std::vector<CarTime> car_times;
  std::vector<Summary> history;
  Summary interval_data;
  Summary summary_data;

  long time_from;

  // citac poctu kroku simulace, ve kterem byla obsazena jedna vybrana bunka
  // z teto hodnoty se pak vypocita hustota dopravy
  long cell_time_occupied;

  float occupancy;

  float density;

  // zde se postupne v kazdem kroku pricita sledovana prumerna rychlost na vozovce
  // z teto hodnoty je pak vypocitana prumerna rychlost v casovem useku
  float mean_speed;

  /**
   * nastaveni zda se maji statistiky tisknout ve forme tabulky,
   * data v tomto formatu mohou byt pouzita pro vykresleni grafu gnuplotem
   * nebo primo v latexu pomoci pgfplots rozsireni
   */
  bool table_format;

  /**
   * Pokud je true, netisknou se na vystup progromau statistiky jednotilivych
   * intervalu
   */
  bool suppress_output;

  Config * config;

  public:

    Statistics(Config * config, bool suppress_output = false);

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
    void logOccupancy(float occupancy);
    void logDensity(float density);

    struct Summary getIntervalData() { return interval_data; }
    struct Summary getSummaryData() { return summary_data; }

    void summaryCalculate();
    void summaryPrint();
    void summaryReset();

    void summaryCalculateAndPrint() {
      summaryCalculate();
      summaryPrint();
    }

    static void printHeader();
};

#endif

