#pragma once
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

namespace Kraken {
    struct KrakenCancelOrder {
        int count;
        bool pending;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(KrakenCancelOrder, count, pending);
};
}