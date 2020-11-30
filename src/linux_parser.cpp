#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float memoryUtilization{0.0};
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  string line;
  string key;
  long value;
  float memtotal{0};
  float memFree{0};

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        memtotal = value;
      } else if (key == "MemFree:") {
        memFree = value;
      };
    };
  };
  memoryUtilization = (memtotal - memFree) / memtotal;
  return memoryUtilization;
}

// Done: Read and return the system uptime
long LinuxParser::UpTime() {
  long upTime{0};
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string line;

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime;
  };
  return upTime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// Done: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::string key = "processes";
  std::string fileName = kProcDirectory + kStatFilename;
  return int(LinuxParser::FindValueInFile(key, fileName));
}

// Done: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::string key = "procs_running";
  std::string fileName = kProcDirectory + kStatFilename;
  return int(LinuxParser::FindValueInFile(key, fileName));
}

// Done: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string line;

  std::getline(stream, line);
  return line;
}

// Review: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::string key = "VmSize:";
  std::string fileName = kProcDirectory + to_string(pid) + kStatusFilename;
  float ramInKb = int(LinuxParser::FindValueInFile(key, fileName));
  char ramInMb[10];
  sprintf(ramInMb, "%.3f", ramInKb / 1000);
  return std::string(ramInMb);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::string key = "Uid:";
  std::string fileName = kProcDirectory + to_string(pid) + kStatusFilename;
  int uid = int(LinuxParser::FindValueInFile(key, fileName));
  return to_string(uid);
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line;
  string lineElement;
  int elementPosition = 22;
  long uptime;

  // Get the clockticks value from stats file
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (elementPosition > 0 && linestream >> lineElement) {
      elementPosition--;
    };
  };

  // Converte clocktickts to seconds 
  uptime = stol(lineElement) / sysconf(_SC_CLK_TCK);

  return uptime;
}

double LinuxParser::FindValueInFile(std::string const& key,
                                    std::string const& fileName) {
  std::ifstream stream(fileName);
  string line;
  string keyFound;
  double value{0.0};

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> keyFound >> value;
      if (keyFound == key) {
        return value;
      };
    };
  };
  return value;
}