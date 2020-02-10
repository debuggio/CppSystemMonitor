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
  stream.close();

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
  string memTotalLabel = "MemTotal:";
  string memFreeLabel = "MemFree:";
  string memBuffersLabel = "Buffers:";

  string value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    float totalMemory = 0.;
    float freeMemory = 0.;
    float buffers = 0.;
    while (std::getline(stream, line))
    {
        if (totalMemory != 0 && freeMemory != 0)
            break;
        if (line.compare(0, memTotalLabel.size(), memTotalLabel) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            totalMemory = stof(values[1]);
        }
        if (line.compare(0, memFreeLabel.size(), memFreeLabel) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            freeMemory = stof(values[1]);
        }
        if (line.compare(0, memBuffersLabel.size(), memBuffersLabel) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            buffers = stof(values[1]);
        }
    }
    if (totalMemory - buffers != 0) {
      return float(1.0 - (freeMemory / (totalMemory - buffers)));
    }
    stream.close();
  }
  return 0.;
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
    stream.close();
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

  ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
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
    stream.close();
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
    while(lineStream >> value) {
      if (value != "cpu") {
        result.push_back(value);
      }
    }
    stream.close();
  }

  return result;
}

float LinuxParser::CpuUtilization(int pid) {  
  string line;
  string value;
  float result = 0.;

  ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream lineStream(line);
    std::istream_iterator<string> beg(lineStream), end;
    vector<string> values(beg, end);

    float utime = stof(values[13]);
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float starttime = stof(values[21]);
    float totalTime = utime + stime + cutime + cstime;
    float hertz = sysconf(_SC_CLK_TCK);
    float seconds = (starttime / hertz);

    result = 100.0 * ((totalTime / hertz) / seconds);

    stream.close();
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
    stream.close();
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
    stream.close();
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
    stream.close();
    return result;
  }

  return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  int value;

  ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while(getline(stream, line)) {
      std::istringstream lineStream(line);
      lineStream >> key >> value;
      if (key == "VmSize:") {
        return to_string(value / 1000);
      }
    }
    stream.close();
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
    stream.close();
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
    stream.close();
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
    int count = 0;
    while(lineStream >> data) {
      if (count == 21) {
        return LinuxParser::UpTime() - (stol(data) / sysconf(_SC_CLK_TCK));
      }
      count++;
    }
    stream.close();
  }

  return result; 
}