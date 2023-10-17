#pragma once

class ActionPoint {
    enum Action { BUY, SELL };
    Action action;
    // Better precision?
    double price;
    // should we add trigger here or on the strategy?
};