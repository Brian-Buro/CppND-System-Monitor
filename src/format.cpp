#include <string>
#include "format.h"

using std::string;

#define timeStrLen 10

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
std::string Format::ElapsedTime(long timeInSeconds) {
    int hours;
    int minutes;
    int seconds;
    char formatedTime[timeStrLen];
    string output;

    // Calculate hours, minutes and seconds from seconds
    hours = int(timeInSeconds / 3600);
    minutes = int(timeInSeconds % 3600) / 60;
    seconds = int(timeInSeconds % 60);

    // Format char array and convert to string
    std::snprintf(formatedTime, timeStrLen ,"%02d:%02d:%02d", hours, minutes, seconds);
    output = formatedTime;
    return output; 
}
