#include "Clock.hpp"

long long Clock::getMillisSinceEpoch() {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
