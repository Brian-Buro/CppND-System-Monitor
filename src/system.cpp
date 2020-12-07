#include "system.h"

#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Redo: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  bool pidNotFound{true};

  std::sort(pids.begin(), pids.end());
  // Add new processes
  for (size_t idx = 0; idx < pids.size(); idx++) {
    for (size_t y = 0; y < processes_.size(); y++) {
      if (processes_[y].Pid() == pids[idx]) {
        pidNotFound = false;
        break;
      }
    }
    if (pidNotFound){
        Process p;
        p.Pid(pids[idx]);
        processes_.push_back(p);
    }
    pidNotFound = true;
  }
  // Remove closed processes if required

  // Sort
  std::sort(processes_.begin(), processes_.end());
  return processes_;
}

// Done: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Done: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Done: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Done: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Done: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }