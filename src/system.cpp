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

// Done: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Done: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  vector<Process> currentProcesses(pids.size());
  bool newProcess{true};

  /******************************************************  
  Populate the new vector of current processes.
  If a process already exists, all information is copied 
  from processes_, otherwise just the pid will be set.
  ******************************************************/
  for (size_t idx = 0; idx < pids.size(); idx++) {
    for (size_t y = 0; y < processes_.size(); y++) {
      if (processes_[y].Pid() == pids[idx]) {
        currentProcesses[idx] = processes_[y];
        newProcess = false;
        break;
      }
    }
    if (newProcess){
        currentProcesses[idx].Pid(pids[idx]);
    }
    currentProcesses[idx].UpdateCpuUtilization();
    newProcess = true;
  }

  // Sort
  std::sort(currentProcesses.begin(), currentProcesses.end());

  // Update processes_
  processes_ = currentProcesses;
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