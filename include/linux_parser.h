#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0, // normal processes executing in user mode
  kNice_,     // niced processes executing in user mode
  kSystem_,   // processes executing in kernel mode
  kIdle_,     // twiddling thumbs
  kIOwait_,   // In a word, iowait stands for waiting for I/O to complete.
  kIRQ_,      // servicing interrupts
  kSoftIRQ_,  // servicing softirqs
  kSteal_,    // involuntary wait
  kGuest_,    // running a normal guest
  kGuestNice_ // running a niced guest
};

// PID
enum PIDStates {
  kPid_ = 0, // process id
  kTComm_,   // filename of the executable
  kState_,   // state (R is running, S is sleeping, D is sleeping in an
             // uninterruptible wait, Z is zombie, T is traced or stopped)
  kPpid_,    // process id of the parent process
  kPgrp_,    // pgrp of the process
  kSid_,     // session id
  kTtyNr_,   // tty the process uses
  kTtyPgrp_, // pgrp of the tty
  kFlags_,   // task flags
  kMinFlt_,  // number of minor faults
  kCminFlt_, // number of minor faults with child's
  kMajFlt_,  // number of major faults
  kCmajFlt_, // number of major faults with child's
  kUtime_,   // user mode jiffies
  kStime_,   // kernel mode jiffies
  kCutime_,  // user mode jiffies with child's
  kCsutime_  // kernel mode jiffies with child's
};

enum PIDStat {
  pid = 0, // process id.
  exName,  // filename of the executable
  state, // R is running, S is sleeping, is sleeping in an uninterruptible wait,
         // Z is zombie, T is traced or stopped
  euid,  // effective user id
  egid,  // effective group id
  ppid,  // The pid of the parent.
  pgrp,  // The pgrp of the process.
  session,       // session id of the process.
  tty,           // tty the process uses
  tpgid,         // (too long)
  flags,         // flags of the process.
  minflt,        // number of minor faults
  cminflt,       // number of minor faults with childs
  majflt,        // number of major faults
  cmajflt,       // number of major faults with childs
  utime,         // user mode jiffies
  stime,         // kernel mode jiffies
  cutime,        // user mode jiffies with childs
  cstime,        // kernel mode jiffies with childs
  counter,       // process's next timeslice
  priority,      // the standard nice value, plus fifteen
  timeout,       // time in jiffies of the next timeout
  itrealvalue,   // time before the next SIGALRM is sent to the process
  starttime,     // time the process started after system boot
  vsize,         // virtual memory size
  rss,           // resident Set Size
  rlim,          // current limit in bytes on the rss
  startcode,     // address above which program text can run
  endcode,       // address below which program text can run
  startstack,    // address of the start of the stack
  kstkesp,       // current value of ESP
  kstkeip,       // current value of EIP
  signal,        // bitmap of pending signals
  blocked,       // bitmap of blocked signals
  sigignore,     // bitmap of ignored signals
  sigcatch,      // bitmap of catched signals
  wchan,         // (too long)
  sched,         // scheduler
  sched_priority // scheduler priority
};

std::vector<std::string> CpuUtilization();
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
float CpuUtilization(int pid);
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
}; // namespace LinuxParser

#endif