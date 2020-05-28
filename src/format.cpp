#include <iomanip>
#include <sstream>
#include <string>

#include "format.h"

using std::string;
using std::stringstream;

// Elapsed time format: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int hour = seconds / (60 * 60);
  int minute = (seconds / 60) % 60;
  int second = seconds % 60;

  stringstream elapsedtime;

  elapsedtime << std::setfill('0') << std::setw(2) << hour << ":";
  elapsedtime << std::setfill('0') << std::setw(2) << minute << ":";
  elapsedtime << std::setfill('0') << std::setw(2) << second;

  return elapsedtime.str();
}