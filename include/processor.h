#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // See src/processor.cpp

  // Declare any necessary private members
 private:
 long totalJiffies = 0; 
 long activeJiffies = 0; 
 
};

#endif