#include <cstdlib>
#include <sstream>
#include <iterator>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

int main() {

  ifstream file("data/samples.csv", ifstream::in);
  char column_separator = ',';
  int track_length = 5349;

  if (!file.good()) {
    cout << "Cannot open samples file 'data/samples/csv'." << endl;
    exit(-1);
  }

  string line;
  map<int, vector<int> > statistics;

  while(getline(file, line)) {

    vector<string> chunks;

    stringstream lineStream(line);
    string cell;

    while(getline(lineStream, cell, column_separator))
      chunks.push_back(cell);

    // 0:data-type, 1:time out, 2:time in, 3:total time, 4:car type in, 5:car type out
    // EVCEO-EVVN,22-06-2012 04:51:16,22-06-2012 04:43:34,462,-1,2

    if (chunks.size() == 6 && chunks.at(0) == "EVCEO-EVVN") {

      int car_class = atoi(chunks.at(5).c_str());
      int expected_time = atoi(chunks.at(3).c_str());

      statistics[car_class].push_back(expected_time);

    } else {
      cout << "Bad line" << line << endl;
    }
  }

  file.close();

  // tisk hlavicky
  for (int i = 0; i < 57; ++i) cout << '-';
  cout << "\n| id\t| #\t| T[s]\t| speed[m/s]\t| speed[km/h]\t|" << endl;
  for (int i = 0; i < 57; ++i) cout << '-'; cout << endl;

  int sum, timelen;
  for (map<int,vector<int> >::iterator it = statistics.begin(); it != statistics.end(); ++it) {

    // tisk id tridy vozidla a poctu vozidel dane tridy v sade
    cout << "| " << it->first << "\t| "
         << it->second.size() << "\t| ";

    // vypocet a tisk prumerne doby dojezdu dane tridy
    sum = 0;
    for(vector<int>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
      sum += *vit;
    timelen = sum / it->second.size();
    cout << timelen << "\t| ";

    // tisk prumerne rychlosti v m/s a v km/h
    cout << float(track_length)/float(timelen) << "\t| "
         << float(track_length)/float(timelen)*3.6 << "\t|" << endl;
  }
  for (int i = 0; i < 57; ++i) cout << '-'; cout << endl;
}
