#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <vector>
#include <string>
using std::vector;

class Processor {
 public:
  float Utilization();  // Clean up: See src/processor.cpp
  static vector< long int> strVecToLong(const vector<std::string> &);

  // TODO: Declare any necessary private members
 private:
  long preIdle_ = 0;
  long preTotal_ = 0;
};

#endif