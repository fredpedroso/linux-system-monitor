#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  std::vector<std::string> cpuUtilization; // CPU Utilization
  float user, nice, system, idle, iowait, irq, softirq, steal; // CPU states
  float nonIdleJiffies, IdleJiffies, totalJiffies;             // Jiffies
  long totalDifferentiate, idleDifferentiate;                  // Differentiate

  // Get CPU utilization
  cpuUtilization = LinuxParser::CpuUtilization();

  // Amount of time the CPU has spent performing different kinds of work
  user = std::stof(cpuUtilization[LinuxParser::CPUStates::kUser_]);
  nice = std::stof(cpuUtilization[LinuxParser::CPUStates::kNice_]);
  system = std::stof(cpuUtilization[LinuxParser::CPUStates::kSystem_]);
  idle = std::stof(cpuUtilization[LinuxParser::CPUStates::kIdle_]);
  iowait = std::stof(cpuUtilization[LinuxParser::CPUStates::kIOwait_]);
  irq = std::stof(cpuUtilization[LinuxParser::CPUStates::kIRQ_]);
  softirq = std::stof(cpuUtilization[LinuxParser::CPUStates::kSoftIRQ_]);
  steal = std::stof(cpuUtilization[LinuxParser::CPUStates::kSteal_]);

  // Idle process
  IdleJiffies = idle + iowait;

  // Non iddle process
  nonIdleJiffies = user + nice + system + irq + softirq + steal;

  // Total process
  totalJiffies = IdleJiffies + nonIdleJiffies;

  // Differentiate: actual value minus the previous one
  totalDifferentiate = totalJiffies - prevTotal_;
  idleDifferentiate = IdleJiffies - prevIdle_;

  // Previous value receive the current due the next calc
  prevTotal_ = totalJiffies;
  prevIdle_ = IdleJiffies;

  // CPU utilization percentage
  cpuUtilization_ =
      (totalDifferentiate - idleDifferentiate) / float(totalDifferentiate);

  return cpuUtilization_;
}