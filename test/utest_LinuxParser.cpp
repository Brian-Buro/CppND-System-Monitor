#include "gtest/gtest.h"
#include "../include/linux_parser.h"
#include "../include/format.h"
#include <string>
#include <iostream>
#include <vector>

#define PID 2016


namespace {

TEST(LinuxParserTest, OperatingSystem){
  std::string OsName = "Ubuntu 20.10";
  EXPECT_EQ(LinuxParser::OperatingSystem(), OsName);
}

TEST(LinuxParserTest, SystemUpTime) {
  long upTime{4000};
  EXPECT_GE(LinuxParser::UpTime(), upTime);
  std::cout << "Uptime: " << LinuxParser::UpTime() << "\n";
}

TEST(LinuxParserTest, MemUtilisation) {
  float defaultValue{0.0};
 // EXPECT_GE(LinuxParser::MemoryUtilization(), defaultValue);
  std::cout << "MemoryUtilization: " << LinuxParser::MemoryUtilization() << "\n";
}


TEST(FormatTest, TimeFormat) {
  long t = 10l;
 // std::string AA = Format::ElapsedTime(t);
 // std::string strUpTime = "00:00:10";
 // EXPECT_EQ(Format::ElapsedTime(uptime), strUpTime);
 // std::cout << AA << " :Uptime ! \n";
}

TEST(Pids, PIDList) {
  std::vector<int> pids = LinuxParser::Pids();

  std::cout << "Pids" << "\n";
  for (int & pid : pids) {
    std::cout << pid << ", ";
  }
  std::cout << "\n";
}

TEST(Pids, cmd) {
  std::string cmdStr ="/home/brian/.vscode/extensions/ms-vscode.cpptools-1.1.2/bin/cpptools\0";
  EXPECT_STREQ(LinuxParser::Command(PID).c_str(), cmdStr.c_str());
}
TEST(Pids, Uid) {
  EXPECT_STREQ(LinuxParser::Uid(PID).c_str(), "1000");
}
TEST(Pids, Uptime) {
  long upTime = LinuxParser::UpTime(PID);
  std::cout << "Uptime: " << upTime << " seconds.\n";
}

};