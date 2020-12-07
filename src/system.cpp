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

// Done: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  vector<Process> currentProcesses(pids.size());
  bool newProcess{true};

  /******************************************************  
  Populate the vector of current processes.
  If a process already exists, it is copied
  from processes_, otherwise a new member in
  the currentProcesses vector will be created
  for it.
  ******************************************************/
  for (size_t idx = 0; idx < pids.size(); idx++) {
    for (size_t y = 0; y < processes_.size(); y++) {
      if (processes_[y].Pid() == pids[idx]) {
        currentProcesses.push_back(processes_[y]);
        newProcess = false;
        break;
      }
    }
    if (newProcess){
        Process p;
        p.Pid(pids[idx]);
        currentProcesses.push_back(p);
    }
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