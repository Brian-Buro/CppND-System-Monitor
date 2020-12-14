#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid();                               
  void Pid(int pid);
  std::string PidFormatted() const;
  std::string User();                      
  std::string Command();                   
  float CpuUtilization();                  
  void UpdateCpuUtilization();
  std::string Ram();                       
  long int UpTime();                       
  bool operator<(Process const& a) const;  

 private:
  int pid_;
  float cpuUtilization_;
  float preActiveTime_{0};
  float preUpTime_{0};
  std::string userName_{""};
  std::string command_{""};
};

#endif