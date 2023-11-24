#include <fstream>
#include <iostream>
#include <string>
#include "KrakenLoader.hpp"

namespace Kraken {
    KrakenLoader::KrakenLoader(const KrakenClient& _client, const std::filesystem::path& path): client(_client), filePath(path / "kraken") {}

    bool KrakenLoader::fetchData(long long int epochMilli, const std::string& pair) {
        // check the file path for a folder by the pair and then epoch milli rounded down to the nearest m inute
        std::chrono::milliseconds duration(epochMilli);
        std::chrono::hours floorHour = std::chrono::floor<std::chrono::hours>(duration);
        std::filesystem::path pairPath = filePath / pair;
        create_directories(pairPath);
        pairPath /= std::to_string(floorHour.count());
        if (std::filesystem::exists(pairPath)) {
            std::cout << "File already exists at " << filePath.c_str() << std::endl;
            std::filesystem::remove(pairPath);
        }
        // increments of 1 minute. We'll save each file by the beggining of the hour. We get 12 hours per API call
        auto ohlcPoints = client.getTrades(std::chrono::duration_cast<std::chrono::milliseconds>(floorHour).count(), pair);
        // now we have to aggregate the trades into increments of 1 minute and save. Keep loading API calls until we have 1 hour worth of data.
        if (std::ofstream file(pairPath); file.is_open()) {
            std::cout << "File created at path: " << pairPath.c_str() << std::endl;
            // Optionally, you can write to the file here if needed.
            file << "Hello, File!" << std::endl;
            file.close();
        }
        return true;
    }

    void KrakenLoader::saveHourOfTrades(std::chrono::hours hour, const std::string&pair) {
        // reads trades for the next hour and saves them to a file
        std::chrono::milliseconds startMillis = std::chrono::duration_cast<std::chrono::milliseconds>(hour);
        std::vector<KrakenTrade> trades = client.getTrades(startMillis.count(), pair);
        auto ohlcPoints = aggregateTradesInMinutes(trades);
        // feeling lazy, so for now we just calculate size of ohlc points to know how many we have
        while (ohlcPoints.size() < 60) {
            std::chrono::milliseconds milliStart = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::duration_cast<std::chrono::minutes>(hour) + std::chrono::minutes(ohlcPoints.size())
                );

            std::vector<KrakenTrade> moreTrades = client.getTrades(milliStart.count(), pair);
            auto points = aggregateTradesInMinutes(moreTrades);
            ohlcPoints.insert(ohlcPoints.end(), points.begin(), points.end());
        }
        ohlcPoints.resize(60);
        std::filesystem::path pairPath = filePath / pair;
        create_directories(pairPath);
        pairPath /= std::to_string(hour.count());
        if (std::ofstream file(pairPath); file.is_open()) {
            std::cout << "File created at path: " << pairPath.c_str() << std::endl;
            // Optionally, you can write to the file here if needed.
            file << "Hello, File!" << std::endl;
            file.close();
        }
    }


    std::vector<KrakenOHLC> KrakenLoader::aggregateTradesInMinutes(const std::vector<KrakenTrade>&trades) const {
        std::chrono::milliseconds duration(static_cast<int>(trades.at(0).getTime()));
        std::chrono::minutes nextMinute = std::chrono::ceil<std::chrono::minutes>(duration);
        KrakenOHLC average{};
        int count = 0;
        std::vector<KrakenOHLC> result;
        std::ranges::for_each(trades, [&](const KrakenTrade& trade) {
            if (count == 0) average.setOpen(trade.getPrice());
            if (trade.getPrice() > average.getHigh()) average.setHigh(trade.getPrice());
            if (trade.getPrice() < average.getLow()) average.setLow(trade.getPrice());
            average.setClose(trade.getPrice());
            average.setVwap(average.getVwap() + trade.getPrice());
            average.setVolume(average.getVolume() + trade.getVolume());
            ++count;
            if (std::chrono::minutes(static_cast<int>(trade.getTime())) >= nextMinute) {
                nextMinute += std::chrono::minutes(1);
                if (count != 0) {
                    average.setVwap(average.getVwap() / count);
                    result.push_back(average);
                }
                average = KrakenOHLC();
                count = 0;
            }
        });
        return result;
    }
}