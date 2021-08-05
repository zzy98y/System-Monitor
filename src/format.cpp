#include <string>
#include <iomanip>
#include "format.h"

using std::string;
using std::setw;
using std::setfill;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    std::ostringstream formatTime;
    const int sec_in_minute = 60; 
    const int sec_in_hour = 3600; 
    long hour = seconds/sec_in_hour;
    long remain = seconds%sec_in_hour;
    long minute = remain/sec_in_minute;
    remain = remain%sec_in_minute;

    formatTime << setfill('0') << setw(2) << hour << ":"
     << setfill('0') << setw(2) << minute << ":" << setfill('0') << setw(2) << remain;
    return formatTime.str();
     }