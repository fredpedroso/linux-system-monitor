#include <dirent.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stoi;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// Get the Linux operating system distribution name
string LinuxParser::OperatingSystem() {
  string distributionName = "Unknown";

  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    string line, key;

    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');

      std::istringstream linestream(line);
      while (linestream >> key >> distributionName) {
        if (key == "PRETTY_NAME") {
          std::replace(distributionName.begin(), distributionName.end(), '_',
                       ' ');
          return distributionName;
        }
      }
    }
  }

  return distributionName;
}

// Get the Linux Kernel version
string LinuxParser::Kernel() {
  string version = "Unknown";

  std::ifstream filestream(kProcDirectory + kVersionFilename);
  if (filestream.is_open()) {
    string line, os, kernel;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel >> version;
  }

  return version;
}

// Get PIDs
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR *directory = opendir(kProcDirectory.c_str());
  struct dirent *file;

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
  float memoryUtilization = 0;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    string line, key, memory;

    float memoryTotal = 0;
    float memoryFree = 0;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);

      while (linestream >> key >> memory) {
        if (key == "MemTotal:") {
          memoryTotal = std::stof(memory);
        } else if (key == "MemFree:") {
          memoryFree = std::stof(memory);
        }
      }
    }

    // Percentagem of system memory utilization
    memoryUtilization = (memoryTotal - memoryFree) / memoryTotal;
  }

  return memoryUtilization;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  long sysUptime = 0;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    string line, uptime;

    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;

    // System uptime value
    sysUptime = stol(uptime);
  }

  return sysUptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long pidActiveJiffies = 0;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    string value, line;

    std::getline(filestream, line);
    std::istringstream linestream(line);
    int count = 0;
    while (linestream >> value) {
      if ((count >= LinuxParser::PIDStat::majflt) &&
          (count <= LinuxParser::PIDStat::stime)) {
        pidActiveJiffies += stol(value);
      }
      count++;
    }
  }

  return pidActiveJiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long sysActiveJiffies = 0;
  vector<string> cpuUtilization{};

  // Get CPU utilization
  cpuUtilization = LinuxParser::CpuUtilization();

  // Get PID status
  long user = std::stol(cpuUtilization.at(LinuxParser::CPUStates::kUser_));
  long nice = std::stol(cpuUtilization.at(LinuxParser::CPUStates::kNice_));
  long system = std::stol(cpuUtilization.at(LinuxParser::CPUStates::kSystem_));
  long irq = std::stol(cpuUtilization.at(LinuxParser::CPUStates::kIRQ_));
  long softirq =
      std::stol(cpuUtilization.at(LinuxParser::CPUStates::kSoftIRQ_));
  long steal = std::stol(cpuUtilization.at(LinuxParser::CPUStates::kSteal_));

  // Total active jiffies
  sysActiveJiffies = user + nice + system + irq + softirq + steal;

  return sysActiveJiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long sysIddleJiffies = 0;
  vector<string> cpuUtilization{};

  // Get CPU utilization
  cpuUtilization = LinuxParser::CpuUtilization();

  // Get PID status
  long idle = std::stol(cpuUtilization.at(LinuxParser::CPUStates::kIdle_));
  long iowait = std::stol(cpuUtilization.at(LinuxParser::CPUStates::kIOwait_));

  // Total iddle jiffies
  sysIddleJiffies = idle + iowait;

  return sysIddleJiffies;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuUtilization;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    string line, utilization;

    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> utilization) {
      if (utilization != "cpu") {
        cpuUtilization.push_back(utilization);
      }
    }
  }

  return cpuUtilization;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int totalProcesses = 0;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    string key, line, process;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> process;
      if (key == "processes") {
        totalProcesses = std::stoi(process);
      }
    }
  }

  return totalProcesses;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int runningProcessess = 0;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    string key, line, process;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> process;

      if (key == "procs_running") {
        runningProcessess = std::stoi(process);
      }
    }
  }

  return runningProcessess;
}

// Read and return CPU utilization used by a process
float LinuxParser::CpuUtilization(int pid) {
  float pidCpuUtilization = 0;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    string line, value;
    long utime, stime, cutime, cstime,
        starttime; // time the CPU has spent performing different kinds of work
    long ticksPerSecond, upTimeSeconds, sysUpTime;
    float pidTotalTime;
    int counter = 0;

    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      if (counter == LinuxParser::PIDStat::utime) {
        utime = std::stol(value);
      }
      if (counter == LinuxParser::PIDStat::stime) {
        stime = std::stol(value);
      }
      if (counter == LinuxParser::PIDStat::cutime) {
        cutime = std::stol(value);
      }
      if (counter == LinuxParser::PIDStat::cstime) {
        cstime = std::stol(value);
      }
      if (counter == LinuxParser::PIDStat::starttime) {
        starttime = std::stol(value);
      }

      ++counter;
    }

    // Total uptime in seconds
    sysUpTime = LinuxParser::UpTime(pid);

    // Total pid time
    pidTotalTime = static_cast<float>(utime + stime + cutime + cstime);

    // Number of clock ticks per second
    ticksPerSecond = sysconf(_SC_CLK_TCK);

    // PID uptime in seconds
    upTimeSeconds = static_cast<long>(sysUpTime - (starttime / ticksPerSecond));

    // CPU usage
    pidCpuUtilization =
        static_cast<float>(((pidTotalTime / ticksPerSecond) / upTimeSeconds));
  }

  return pidCpuUtilization;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string pidCommand = "";

  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    string line;
    if (std::getline(filestream, line))
      pidCommand = line;
  }

  return pidCommand;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string pidMemoryUsed = "0";

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    string line, key, memory;
    std::stringstream ram;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> memory;
      if (key == "VmSize:") {
        ram << std::fixed << std::setprecision(2) << stof(memory) / 1024;
        pidMemoryUsed = ram.str();
        break;
      }
    }
  }

  return pidMemoryUsed;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string pidUUID = "-";

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    string line, key, uuid;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> uuid;
      if (key == "Uid:") {
        pidUUID = uuid;
        break;
      }
    }
  }
  return pidUUID;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string pidUser = "-";

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    string line;
    string user, uidstr, extra;

    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> uidstr >> extra) {
        if (extra == Uid(pid)) {
          pidUser = user;
          break;
        }
      }
    }
  }

  return pidUser;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long pidUpTime = 0;
  long ticksPerSecond = 0;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::string line, value;
    long startTime;

    std::getline(filestream, line);
    std::istringstream iss(line);
    int count = 0;
    while (iss >> value) {
      if (count == LinuxParser::PIDStat::starttime) {
        startTime = std::stol(value);
        break;
      }
      count++;
    }

    ticksPerSecond = sysconf(_SC_CLK_TCK); // Number of clock ticks per second
    pidUpTime = UpTime() - (startTime / ticksPerSecond);
  }

  return pidUpTime;
}