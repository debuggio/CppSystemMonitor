#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;
using std::ifstream;
using std::getline;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os;
  string kernel;
  string _;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> _ >> kernel;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  float value;

  float memTotal;
  float memFree;
  float result = 0L;
  
  ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while(getline(stream, line)) {
        std::istringstream lineStream(line);
        lineStream >> key >> value;
          if (key == "MemTotal")
              memTotal = value;
          else if (key == "MemFree")
              memFree = value;
    }
    result = (memTotal - memFree) / memTotal;
  }
  
  return result;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  long result = 0L;
  
  ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream lineStream(line);
    lineStream >> result;
  }

  return result;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long result = 0L;
  vector<string> ticks = LinuxParser::CpuUtilization();
  for (int state = kUser_; state <= kSteal_; state++) {
      result += stol(ticks[state]);
  }
  
  return result;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string value;
  long result = 0L;

  ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    getline(stream, line);
    int count = 0;
    std::istringstream lineStream(line);
    while (lineStream >> value) {
      if (count >= 13 && count <= 16) {
        result += stol(value);
      } else if (count >= 17) {
        break;
      }
      count++;
    }
  }
  return result;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long result = 0L;
  vector<string> ticks = LinuxParser::CpuUtilization();
  for (int state = kUser_; state <= kSteal_; state++) {
    if (state != kIdle_ && state != kIOwait_) {
      result += stol(ticks[state]);
    }
  }

  return result;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long result = 0L;
  vector<string> cpuUtilization = LinuxParser::CpuUtilization();
  for(int i = kIdle_; i <= kIOwait_; ++i) {
    result += stof(cpuUtilization[i]);
  }

  return result;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {  
  string line;
  string value;
  vector<string> result {};

  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream lineStream(line);
    lineStream >> value;
    while(lineStream >> value) {
      result.push_back(value);
    }
  }

  return result;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  int result = 0;

  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(getline(stream, line)) {
      std::istringstream lineStream(line);
      if (lineStream >> key >> result && key == "processes") {
        return result;
      }
    }
  }

  return result;  
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  int result = 0;

  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(getline(stream, line)) {
      std::istringstream lineStream(line);
      if (lineStream >> key >> result && key == "procs_running") {
        return result;
      }
    }
  }

  return result;   
 }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string result;
  ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    getline(stream, result);
    return result;
  }

  return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  string value;

  ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while(getline(stream, line)) {
      std::istringstream lineStream(line);
      lineStream >> key >> value;
      if (key == "Uid:") {
        return value;
      }
    }
  }
  
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value;

  ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while(getline(stream, line)) {
      std::istringstream lineStream(line);
      lineStream >> key >> value;
      if (key == "Uid:") {
        return value;
      }
    }
  }
  
  return string();  
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  string line;
  string key;
  string _;
  string value;

  ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while(getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      lineStream >> key >> _ >> value;
      if (value == uid) {
        return key;
      }
    }
  }
  
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line;
  string data;
  long result = 0L;

  ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream lineStream(line);
    int i = 0;
    while(lineStream >> data) {
      if (i == 21) {
        return stol(data) / sysconf(_SC_CLK_TCK);
      }
      i++;
    }
  }

  return result; 
}