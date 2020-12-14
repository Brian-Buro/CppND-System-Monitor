#include <iostream>
#include <unistd.h>

#include "../include/process.h"
#include "../include/linux_parser.h"
#include "../include/format.h"

#define pid 854

int main() {

    Process pro;
    pro.Pid(pid);
    long uptime = LinuxParser::UpTime(pid);

    std::cout << "User: " << LinuxParser::User(pid) << "\n";
    
    std::cout << "PID: " << pid << "\n";
    std::cout << "Clock Fqz: " << sysconf(_SC_CLK_TCK) << "\n";

    std::cout << "Cpu Utilisation: " << pro.CpuUtilization() << "\n";

    std::cout << "ActiveJiffies: " <<  LinuxParser::ActiveJiffies(pid)  << "\n";
    std::cout << "Process Uptime [s]: " <<  uptime  << "\n";
    std::cout << "Process Uptime: " <<  Format::ElapsedTime(uptime)  << "\n";
    std::cout << "System Uptime [s]: " <<  LinuxParser::UpTime()  << "\n";
    std::cout << "System Uptime: " <<  Format::ElapsedTime(LinuxParser::UpTime())  << "\n";
}