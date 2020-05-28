#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
public:
  Processor() : prevTotal_(0), prevIdle_(0) {}
  float Utilization();

private:
  float cpuUtilization_;
  long prevTotal_;
  long prevIdle_;
};

#endif