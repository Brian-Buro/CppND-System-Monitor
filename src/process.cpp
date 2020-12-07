#include "process.h"

#include <linux_parser.h>
#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

// Done: Return this process's ID
int Process::Pid() { return pid_; }
void Process::Pid(int pid) { pid_ = pid; };
std::string Process::PidFormatted() const {
  const size_t bufferLength = 7;
  char pidFormatted[bufferLength];
  // Fieldwidth is the char array lenght -1, for /O. Fieldwidth is negitive for left justification
  int fieldWidth = -1*static_cast<int>(bufferLength-1); 

  snprintf(pidFormatted, bufferLength, "%*d", fieldWidth, pid_);
  return std::string(pidFormatted);
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
  float activeJiffies = LinuxParser::ActiveJiffies(pid_);
  float upTime = LinuxParser::UpTime(pid_);  // Time duration for which the process has run [Seconds]

  float activeTime = activeJiffies / sysconf(_SC_CLK_TCK);

  float changeInActiveTime = activeTime - preActiveTime_;
  float timeStep = upTime - preUpTime_;

  cpuUtilization_ = changeInActiveTime / timeStep;

  // Update the pre values for the next function call.
  preActiveTime_ = activeTime;
  preUpTime_ = upTime;

  return cpuUtilization_;
}

// Done: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Done: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// Redo, could extend with names: Return the user (name) that generated this
// process
string Process::User() { return LinuxParser::Uid(pid_); }

// Done: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
// Sort uses operator< to sort in ascending order, here it's overloaded with >
// to achive decending order
bool Process::operator<(Process const& a) const {
  return cpuUtilization_ > a.cpuUtilization_;
}