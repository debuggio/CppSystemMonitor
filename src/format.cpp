#include <string>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    string secondsStr = seconds%60 < 10 ? "0" + to_string(seconds%60) : to_string(seconds%60);
    
    int minutes = seconds / 60;
    string minutesStr = minutes < 10 ? "0" + to_string(minutes) : to_string(minutes);
    
    int hours = minutes / 60;
    string hoursStr = hours < 10 ? "0" + to_string(hours) : to_string(hours);

    return "" + hoursStr + ":" + minutesStr + ":" + secondsStr;
}