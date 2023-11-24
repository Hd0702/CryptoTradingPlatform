#include "BuyAndHold.hpp"

BuyAndHold::BuyAndHold(std::shared_ptr<BaseExchange> ex): BaseStrategy(ex) {}

bool BuyAndHold::buy() {
    throw std::logic_error("Function not implemented yet");
}

bool BuyAndHold::sell() {
    throw std::logic_error("Function not implemented yet");
}