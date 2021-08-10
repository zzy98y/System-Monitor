#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line; 
  string key; 
  string unit; 
  long memTotal = 0; 
  long memFree = 0; 
  long memValue; 

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream,line)) {
    std::istringstream linestream(line);
    linestream >> key >> memValue >> unit;
    if (key == "MemTotal:") {
      memTotal = float(memValue);
    }
    if (key == "MemFree:") {
      memFree = float(memValue);
    }
  }
  float memUsed = memTotal - memFree;
  float Utilization = memUsed/memTotal;
  return Utilization;

  }
  return 0;
  
   }


// Read and return the system uptime
long LinuxParser::UpTime() { 
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  long upTime = 0; 
  string line; 
  if (filestream.is_open()) {
    std::getline(filestream,line);
    std::istringstream linestream(line);
    linestream >> upTime;
    return upTime;
  }
  
  return upTime;
   }

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
    long totalJiffies = 0;
  vector<string> cpuUtilization = LinuxParser::CpuUtilization();
  for(int i = kUser_; i <= kSteal_; i++) {
    totalJiffies += stol(cpuUtilization[i]);
  }
  return totalJiffies; 
}
// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  string key; 
  string line; 
  long int value = 0; 
  long int activeJiffles = 0; 
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()){ 
    std::getline(filestream,line);
    std::istringstream linestream(line);
    for (int i = 0; i < 13; ++i) {
      linestream >> key;
    }
    for (int i = 0; i < 4; ++i) {
      linestream >> value; 
      activeJiffles += value;
    }
    return activeJiffles;
  }
  
  return activeJiffles;
   }

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long int activeJiffies = 0; 
  activeJiffies = LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
  return activeJiffies; }

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long int total  = 0;
  vector<string> CpuUtilization = LinuxParser::CpuUtilization();
  for (int i = kIdle_; i < kIOwait_; i++) {
    total += std::stol(CpuUtilization[i]);
  } 
  return total; 
  }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line; 
  string value;
  vector<string> cpuValue;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream,line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      if (value != "cpu") {
        cpuValue.push_back(value);
      }

    }
  }
  return cpuValue; 
  }

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string key;
  string line;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
    std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return std::stoi(value);
        }
      }
    }
  }
  return std::stoi(value);
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string key; 
  string line;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) { 
    std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
     }
    
  }
  return std::stoi(value);
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
   string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string key; 
  string line;
  float value;
  std::stringstream ram;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
      while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
         ram << std::fixed << value/ 1000;
         return ram.str();
        }
      }
    }
  }
  return "0";
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string key; 
  string line; 
  string value; 
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream,line)) {
    std::istringstream linestream(line);
    linestream >> key >> value;
    if (key == "Uid:") {
      return value;
    }
  }
  }
  
  return value; 
  }

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string uid = LinuxParser::Uid(pid);
  string line; 
  string key; 
  string value; 
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    std::getline(filestream,line);
    std::replace(line.begin(),line.end(),':',' ');
    std::replace(line.begin(),line.end(),'x',' ');
    std::istringstream linestream(line);
    if (linestream >> key >> value) {
      if (value == uid) {
        return key;
      }
    }
  }
  
  return key; 
  }

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string key;
  string line; 
  long value;
  long upTimePid;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream,line)) {
    std::istringstream linestream(line);
    for (int i = 0; i < 21; ++i) {
      linestream >> key;
    }

    linestream >> value; 
    upTimePid = LinuxParser::UpTime() - (value / sysconf(_SC_CLK_TCK));

  } 
  }
  
  return upTimePid; }




