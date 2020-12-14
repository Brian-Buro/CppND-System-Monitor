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

// Done: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
// MemoryUtiliation calculates the current memory utilization from the /proc/memInfo file
  std::string memInfoFileName{kProcDirectory + kMeminfoFilename};

  float memtotal = LinuxParser::FindValueInFile("MemTotal:", memInfoFileName);
  float memFree = LinuxParser::FindValueInFile("MemFree:", memInfoFileName);
  float memoryUtilization = (memtotal - memFree) / memtotal;

  return memoryUtilization;
}

// Done: Read and return the system uptime
long LinuxParser::UpTime() {
/** Read the system uptime from /proc/uptime
 * uptime contains just two floats and only the first one is required.
 */

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

// Done: Read and return the number of jiffies for the system
// This was handled with CpuUtilization()
// long LinuxParser::Jiffies() { return 0; }

// Done: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
/** Parses /proc/<pid>/stat for all active Jiffy counts
 *  and adds them together.
*/
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line;

  // If the file can't be opened return 0
  if (stream.is_open()) {
    std::getline(stream, line);
  } else {
    return 0;
  }

  // Get the 4 required Jiffy counts
  long utime = GetValueByPosition(line, 14);  // CPU Time spent in user mode
  long stime = GetValueByPosition(line, 15);  // CPU Time spent in kernel mode
  long cutime = GetValueByPosition(line, 16); // Waited for children's CPU Time user mode
  long sutime = GetValueByPosition(line, 17); // Waited for children's CPU Time kernel mode

  // Source: https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  long activeJiffies = utime + stime + cutime + sutime;

  return activeJiffies;
}

long LinuxParser::GetValueByPosition(const string& line, int position) {
/** Returns the word at the requied position in a string.
 *  Inputs: line, a string of words seperated with empty spaces.
 *          position, the position of the requird word.
 *  Numbers are considered as words.
 *  E.g.
 *  line: "I am 1.8 meters tall."
 *  position 2 -> 1.8, position 3 -> meters.
 */
  long valueByPosition;
  std::istringstream stream(line);
  string strValue;

  while (position > 0 && stream >> strValue) {
    position--;
  }
  valueByPosition = stol(strValue);
  return valueByPosition;
}

// TODO: Read and return the number of active jiffies for the system
// This was handled with CpuUtilization()
// long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
// This was handled with CpuUtilization()
// long LinuxParser::IdleJiffies() { return 0; }

// Done: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
/** Parses /proc/stat for all cpu utilitsation data.
 * Returns a vector with all cpu data.
 * 
 * Each line in the file is read, until the line starting
 * with "cpu" is found. All numbers in this line are entered
 * to the output vector.
*/
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
string LinuxParser::Command(int pid) {
/** The process command is the only line in /proc/<pid>/cmdline
 *  The NULL char is removed and the cmd sting padded with empty
 *  spaces. This is because ncures only updates new chars and if 
 *  a shorter string is printed, the end of the old string will 
 *  remain.
 */
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  std::string line;
  if (stream.is_open()) {
    std::getline(stream, line);
  }

  if (line.size() > 1) line.pop_back();
  line += std::string(200, ' ');
  return line;
}

// Done: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
/** The ram useage is read in and converted to MB and
 *  formated
 */
  std::string fileName = kProcDirectory + to_string(pid) + kStatusFilename;
  std::string key = "VmSize:";
  float ramInKb = LinuxParser::FindValueInFile(key, fileName);

  const size_t bufferLength = 8;
  char ramInMb[bufferLength];
  int fieldWidth = static_cast<int>(bufferLength - 1);
  snprintf(ramInMb, bufferLength, "%*.3f", fieldWidth, ramInKb / 1000);

  return std::string(ramInMb); // Convert char array to sting
}

// Done: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::string key = "Uid:";
  std::string fileName = kProcDirectory + to_string(pid) + kStatusFilename;
  std::string uid = "";

  int uidInt = LinuxParser::FindValueInFile(key, fileName);
  if (uidInt >= 0) uid = to_string(uidInt); // FindValueInFile returns -1 if key not found.
  return uid;
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) { 
/** Find the User name for a pid
 *  
 *  Go through each line of /etc/passwd untill the pid is
 *  found in position 2 in the line. Once the pid is found
 *  get the user name in postion 4. 
 * 
 *  The user name sometimes contains other chars, which need
 *  to be removed and it must be padded for the same reason 
 *  in Command()
 */  
  std::string uid = LinuxParser::Uid(pid);
  std::string usrName(6, ' ');

  if (uid.empty()) return usrName;

  std::ifstream stream(kPasswordPath);
  std::string line;
  std::string values;

  const int posUid{2};
  const int posName{4};
  bool uidFound{false};

  // If the file can't be opened return
  if (!stream.is_open()) return usrName;

  // Get the user name, assign to values
  while (!uidFound && std::getline(stream, line)) {
    std::istringstream linestream(line);
    for (int posCur = 0; posCur <= posName; posCur++) {
      std::getline(linestream, values, ':');
      if (posCur == posUid && uid.compare(values) == 0) uidFound = true;
    }
  }

  // Copy each char in values upto a "," or "NULL"
  for (size_t idx = 0; idx <= usrName.size() - 1; idx++) {
    if (values[idx] == ',' || values[idx] == '\000') break;
    usrName[idx] = values[idx];
  }
  return usrName;
}

// Done: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line;
  long startTime;
  long uptime;

  // Get the clockticks value from stats file
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    startTime = GetValueByPosition(line, 22);
  };

  // Converte clocktickts to seconds
  uptime = UpTime() - (startTime / sysconf(_SC_CLK_TCK));

  return uptime;
}

double LinuxParser::FindValueInFile(std::string const& key,
                                    std::string const& fileName) {
/** Used for parsing files contining a key value pair on each line.
 *  Inputs: key, name of the required value
 *          fileName, file to seach in
 * 
 *  E.g. The file personHeight looks like this:
 *    John 1.7
 *    Tom 2
 *    Sally 1.6
 *  FindValueInFile(Sally, personHeight) returns 1.6
*/
  std::ifstream stream(fileName);
  string line;
  string keyFound;
  double value{-1};

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