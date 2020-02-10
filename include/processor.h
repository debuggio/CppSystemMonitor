#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

class Processor {
  public:
    float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
  private:
    float User();
    float Nice();
    float System();
    float Idle();
    float IOwait();
    float IRQ();
    float SoftIRQ();
    float Steal();
    float Guest();
    float GuestNice();
};

#endif