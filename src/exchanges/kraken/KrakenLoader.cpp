#include <fstream>
#include <iostream>
#include <string>
#include "KrakenLoader.hpp"

namespace Kraken {
    KrakenLoader::KrakenLoader(const KrakenClient& _client, const std::filesystem::path& path): client(_client), filePath(path / "kraken") {}

    KrakenLoader::KrakenLoader(const KrakenLoader& loader): client(loader.client), filePath(loader.filePath) {}
    // Aggregates trades into minutes and saves them to a file
    std::vector<KrakenOHLC> KrakenLoader::queryAndSaveHourOfTrades(std::chrono::hours hour, const std::string&pair) const {
        // reads trades for the next hour and saves them to a file
        std::chrono::seconds startSeconds = std::chrono::duration_cast<std::chrono::seconds>(hour);
        std::vector<KrakenTrade> trades = client.getTrades(startSeconds.count(), pair);
        auto ohlcPoints = aggregateTradesInMinutes(trades);
        // feeling lazy, so for now we just calculate size of ohlc points to know how many we have
        while (ohlcPoints.size() < 60) {
            std::chrono::seconds secondsStart = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::duration_cast<std::chrono::minutes>(hour) + std::chrono::minutes(ohlcPoints.size())
                );
            if (secondsStart > std::chrono::system_clock::now().time_since_epoch()) {
                std::cout << std::format("Not enough trades to fill an hour for hour {} \n", hour.count());
                return ohlcPoints;
            }
            std::vector<KrakenTrade> moreTrades = client.getTrades(secondsStart.count(), pair);
            auto points = aggregateTradesInMinutes(moreTrades);
            ohlcPoints.insert(ohlcPoints.end(), points.begin(), points.end());
        }
        ohlcPoints.resize(60);
        std::filesystem::path pairPath = filePath / pair;
        create_directories(pairPath);
        pairPath /= std::to_string(hour.count());
        if (std::ofstream file(pairPath); file.is_open()) {
            nlohmann::json j_vec;
            std::ranges::for_each(ohlcPoints, [&](const KrakenOHLC& ohlc) {
                j_vec.push_back(ohlc);
            });
            std::cout << "File created at path: " << pairPath.c_str() << std::endl;
            // Optionally, you can write to the file here if needed.
            file << std::setw(4) << j_vec << std::endl;
            file.close();
        }
        return ohlcPoints;
    }

    // we should probably unit test this
    std::vector<KrakenOHLC> KrakenLoader::aggregateTradesInMinutes(const std::vector<KrakenTrade>&trades) const {
        std::chrono::seconds duration(static_cast<int>(trades.at(0).getTime()));
        std::chrono::minutes nextMinute = std::chrono::ceil<std::chrono::minutes>(duration);
        KrakenOHLC average{};
        int count = 0;
        double volume = 0;
        std::vector<KrakenOHLC> result;
        // get average for each minute
        std::ranges::for_each(trades, [&](const KrakenTrade& trade) {
            if (count == 0) average.setOpen(trade.getPrice());
            if (trade.getPrice() > average.getHigh()) average.setHigh(trade.getPrice());
            if (trade.getPrice() < average.getLow()) average.setLow(trade.getPrice());
            volume += trade.getVolume();
            average.setClose(trade.getPrice());
            average.setVwap(average.getVwap() + trade.getPrice() * trade.getVolume());
            average.setVolume(average.getVolume() + trade.getVolume());
            ++count;
            if (std::chrono::seconds(static_cast<int>(trade.getTime())) >= nextMinute) {
                if (count != 0) {
                    average.setVwap(average.getVwap() / volume);
                    average.setCount(count);
                    average.setTime(std::chrono::duration_cast<std::chrono::seconds>(nextMinute).count());
                    result.push_back(average);
                    count = 0;
                    volume = 0;
                }
                nextMinute += std::chrono::minutes(1);
                average = KrakenOHLC();
            }
        });
        return result;
    }
}
