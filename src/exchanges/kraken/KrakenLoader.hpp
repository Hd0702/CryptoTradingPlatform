#pragma once
#include <filesystem>
#include <string>
#include "KrakenClient.hpp"
namespace Kraken {
    class KrakenLoader {
    public:
        explicit KrakenLoader(const KrakenClient& _client, const std::filesystem::path& = "data_dir/kraken");

        // TODO: Make this chrono?
        bool fetchData(long long int epochMilli, const std::string& pair);

    private:
        std::vector<KrakenOHLC> aggregateTradesInMinutes(const std::vector<KrakenTrade>&trades) const;
        void saveHourOfTrades(std::chrono::hours hour, const std::string& pair);
        std::filesystem::path filePath;
        KrakenClient client;
    };
}