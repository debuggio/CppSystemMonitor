#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

class Processor {
  public:
    float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
  private:
    int _pid{0};
    float _cpuUtilization{0.};
    long _activeJiffies{0L};
    long _totalJiffies{0L};
};

#endif