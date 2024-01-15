#pragma once
#include <filesystem>
#include "KrakenClient.hpp"
namespace Kraken {
    class KrakenLoader {
    public:
        explicit KrakenLoader(const KrakenClient& _client, const std::filesystem::path& = "data_dir/kraken");

        // TODO: Make this chrono?
        std::vector<KrakenOHLC> fetchData(long long int epochSeconds, const std::string&pair);

    private:
        [[nodiscard]] std::vector<KrakenOHLC> aggregateTradesInMinutes(const std::vector<KrakenTrade>&trades) const;

        std::vector<KrakenOHLC> saveHourOfTrades(std::chrono::hours hour, const std::string&pair);
        std::filesystem::path filePath;
        KrakenClient client;
    };
}