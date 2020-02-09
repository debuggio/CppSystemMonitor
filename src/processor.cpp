#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    long totalJiffies = LinuxParser::Jiffies();
    long activeJiffies = LinuxParser::ActiveJiffies();
    long result = (activeJiffies - _activeJiffies)/(totalJiffies - _totalJiffies);
    _activeJiffies = activeJiffies;
    _totalJiffies = totalJiffies;
    return result;
 }