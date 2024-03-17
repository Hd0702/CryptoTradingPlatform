#include "BuyAndHold.hpp"

#include "time/RealClock.hpp"

BuyAndHold::BuyAndHold(const Kraken::KrakenClient& ex): BaseStrategy(ex, Kraken::KrakenLoader(ex), std::make_unique<RealClock>(RealClock())) {}

void BuyAndHold::buy() {
    throw std::logic_error("Function not implemented yet");
}

void BuyAndHold::sell() {
    throw std::logic_error("Function not implemented yet");
}