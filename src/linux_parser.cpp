#include "linux_parser.h"

#include <dirent.h>
#include <stdio.h>
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
long LinuxParser::ActiveJiffies(int pid) {
  long activeJiffies;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line;

  if (stream.is_open()) {
    std::getline(stream, line);
  } else {
    return 0;
  }

  long int utime = GetValueByPosition(line, 14);
  long int stime = GetValueByPosition(line, 15);
  long int cutime = GetValueByPosition(line, 16);
  long int sutime = GetValueByPosition(line, 17);

  // Source:
  // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  activeJiffies = utime + stime + cutime + sutime;

  return activeJiffies;
}

long int LinuxParser::GetValueByPosition(const string& line, int position) {
  long int valueByPosition;
  std::istringstream stream(line);
  string strValue;

  while (position > 0 && stream >> strValue) {
    position--;
  }
  valueByPosition = stol(strValue);
  return valueByPosition;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line;
  string key;
  vector<string> cpuUtilization(10);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "cpu") {
        for (size_t idx = 0; idx < cpuUtilization.size(); idx++) {
          linestream >> key;
          cpuUtilization[idx] = key;
        }
        break;
      };
    }
  }
  return cpuUtilization;
}

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

// Redo: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  std::string line;
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line + std::string(200, '_');
}

// Done: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::string key = "VmSize:";
  std::string fileName = kProcDirectory + to_string(pid) + kStatusFilename;
  float ramInKb = int(LinuxParser::FindValueInFile(key, fileName));
  const size_t bufferLength = 8;
  char ramInMb[bufferLength];
  int fieldWidth = static_cast<int>(bufferLength - 1);
  snprintf(ramInMb, bufferLength, "%*.3f", fieldWidth, ramInKb / 1000);
  return std::string(ramInMb);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::string key = "Uid:";
  std::string fileName = kProcDirectory + to_string(pid) + kStatusFilename;
  int uid = static_cast<int>(LinuxParser::FindValueInFile(key, fileName));

  std::ifstream stream(kPasswordPath);
  std::string line;
  std::string values;
  std::string usrName(6, '_');

  const int posUid{2};
  const int posName{4};
  bool uidFound{false};

  if (!stream.is_open()) return usrName;

  while (!uidFound && std::getline(stream, line)) {
    std::istringstream linestream(line);
    for (int posCur = 0; posCur <= posName; posCur++) {
      std::getline(linestream, values, ':');
      if (posCur == posUid && std::stoi(values) == uid) uidFound = true;
    }
  }

  for (size_t idx = 0; idx <= usrName.size()-1 ; idx++) {
    if (values[idx] == ',') break;
    usrName[idx] = values[idx];
  }
  return usrName;
}

/*
string LinuxParser::Uid(int pid) {
  std::string key = "Uid:";
  std::string fileName = kProcDirectory + to_string(pid) + kStatusFilename;
  int uid = static_cast<int>(LinuxParser::FindValueInFile(key, fileName));

  std::ifstream stream(kPasswordPath);
  std::string line;
  std::string values;
  std::string usrName(6, ' ');

  const int posUid{2};
  const int posName{4};
  int posCur{0};
  bool uidFound{false};

  if (!stream.is_open()) return usrName;

  while (std::getline(stream, line)) {
    std::istringstream linestream(line);
    while (posCur <= posName && std::getline(linestream, values, ':')) {
      if (posCur == posUid && std::stoi(values) == uid) uidFound = true;
      if (uidFound && posCur == posName) {
        while (values.back() == ',') {
          values.pop_back();
        }
        usrName = values.substr(0, 6);
      }
      posCur++;
    }
    if (uidFound) break;
    posCur = 0;
  }


  return usrName;
}
*/

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process  --> use getvaluebyposstion
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line;
  string startTime;
  int elementPosition = 22;
  long uptime;

  // Get the clockticks value from stats file
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (elementPosition > 0 && linestream >> startTime) {
      elementPosition--;
    };
  };

  // Converte clocktickts to seconds
  uptime = UpTime() - (stol(startTime) / sysconf(_SC_CLK_TCK));

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