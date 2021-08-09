#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() { 
    totalJiffies = LinuxParser::Jiffies();
    activeJiffies = LinuxParser::ActiveJiffies();

    if (totalJiffies == 0) {
        return 0.0;
    }

    return float(activeJiffies) / float(totalJiffies);
    
 }