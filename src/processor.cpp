#include "processor.h"

#include <string>
#include <vector>

#include "linux_parser.h"

using LinuxParser::CPUStates;

// Reference:
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
float Processor::Utilization() {
  float utilization;
  std::vector<long int> cpuUtilization;
  long int idle;
  long int diffIdle;
  long int nonIdle;
  long int diffTotal;
  long int total;

  cpuUtilization = strVecToLong(LinuxParser::CpuUtilization());

  idle = cpuUtilization[CPUStates::kIdle_] + cpuUtilization[CPUStates::kIOwait_];
  nonIdle = cpuUtilization[CPUStates::kUser_] + 
            cpuUtilization[CPUStates::kNice_] + 
            cpuUtilization[CPUStates::kSystem_] + 
            cpuUtilization[CPUStates::kIRQ_] + 
            cpuUtilization[CPUStates::kSoftIRQ_] + 
            cpuUtilization[CPUStates::kSteal_];
  
  total = idle + nonIdle;

  diffTotal = total - preTotal_;
  diffIdle = idle - preIdle_;

  utilization = (float(diffTotal) - float(diffIdle)) / float(diffTotal);

  preIdle_ = idle;
  preTotal_ = total;

  return utilization;
}

vector<long int> Processor::strVecToLong(const vector<std::string> &strVec) {
  vector<long int> longVec(strVec.size());

  for (size_t idx = 0; idx < strVec.size(); idx++ ) {
    longVec[idx] = stol(strVec[idx]);
  };
  return longVec;
}