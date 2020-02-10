#include <string>

#include "processor.h"

using std::vector;
using std::stoi;
using std::string;

const vector<string> cpuUtilizationValues {LinuxParser::CpuUtilization()};

float Processor::User() { return std::stoi(cpuUtilizationValues.at(0)); }
float Processor::Nice() { return std::stoi(cpuUtilizationValues.at(1)); }
float Processor::System() { return std::stoi(cpuUtilizationValues.at(2)); } 
float Processor::Idle() { return std::stoi(cpuUtilizationValues.at(3)); }
float Processor::IOwait() { return std::stoi(cpuUtilizationValues.at(4)); }
float Processor::IRQ() { return std::stoi(cpuUtilizationValues.at(5)); }
float Processor::SoftIRQ() { return std::stoi(cpuUtilizationValues.at(6)); }
float Processor::Steal() { return std::stoi(cpuUtilizationValues.at(7)); }
float Processor::Guest() { return std::stoi(cpuUtilizationValues.at(8)); }
float Processor::GuestNice() { return std::stoi(cpuUtilizationValues.at(9)); }

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float idle = Idle() + IOwait();
    float nonIdle = User() + Nice() + System() + IRQ() + SoftIRQ() + Steal();
    float total = idle + nonIdle;
    return (total - idle) / total;
 }