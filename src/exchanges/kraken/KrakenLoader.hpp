#pragma once
#include <fstream>
#include <filesystem>
#include <iostream>
#include "KrakenClient.hpp"
#include "time/time_concepts.hpp"

namespace Kraken {
    class KrakenLoader {
    public:
        explicit KrakenLoader(const KrakenClient& _client, const std::filesystem::path& = "exchange_files");
        explicit KrakenLoader(const KrakenLoader& loader);

        // Get trades for per a minute for the hour contained within epochSeconds
        template<typename T>
        std::vector<KrakenOHLC> fetchData(T timePeriod, const std::string&pair) const {
            // check the file path for a folder by the pair and then epoch milli rounded down to the nearest m inute
            std::chrono::hours floorHour = std::chrono::floor<std::chrono::hours>(timePeriod);
            // check if next hour is past right now, if so we don't need to make a file
            std::chrono::hours nextHour = floorHour + std::chrono::hours(1);
            if (nextHour > std::chrono::floor<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch())) {
                return queryAndSaveHourOfTrades(floorHour, pair);
            }
            std::filesystem::path pairPath = filePath / pair;
            create_directories(pairPath);
            pairPath /= std::to_string(floorHour.count());
            std::cout << "File already exists at " << filePath.c_str() << std::endl;
            std::vector<KrakenOHLC> ohlcPoints;
            if (std::filesystem::exists(pairPath)) {
                std::ifstream file(pairPath);
                if (!file.is_open()) {
                    throw std::runtime_error("File could not be opened");
                }
                nlohmann::json j;
                file >> j;
                for (auto iter = j.begin(); iter != j.end(); ++iter) {
                    KrakenOHLC obj;
                    from_json(*iter, obj); // Convert JSON array to CustomObject
                    ohlcPoints.push_back(obj);
                }
            } else {
                ohlcPoints = queryAndSaveHourOfTrades(floorHour, pair);
            }
            return ohlcPoints;
        }

    private:
        [[nodiscard]] std::vector<KrakenOHLC> aggregateTradesInMinutes(const std::vector<KrakenTrade>&trades) const;

        std::vector<KrakenOHLC> queryAndSaveHourOfTrades(std::chrono::hours hour, const std::string&pair) const;
        std::filesystem::path filePath;
        KrakenClient client;
    };
}
