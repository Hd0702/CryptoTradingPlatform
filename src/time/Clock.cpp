#include "Clock.hpp"

long long Clock::GetMillisSinceEpoch() {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
