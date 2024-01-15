#include <fstream>
#include <iostream>
#include <string>
#include "KrakenLoader.hpp"

namespace Kraken {
    KrakenLoader::KrakenLoader(const KrakenClient& _client, const std::filesystem::path& path): client(_client), filePath(path / "kraken") {}

    std::vector<KrakenOHLC> KrakenLoader::fetchData(long long int epochSeconds, const std::string&pair) {
        // check the file path for a folder by the pair and then epoch milli rounded down to the nearest m inute
        std::chrono::seconds duration(epochSeconds);
        std::chrono::hours floorHour = std::chrono::floor<std::chrono::hours>(duration);
        std::filesystem::path pairPath = filePath / pair;
        create_directories(pairPath);
        pairPath /= std::to_string(floorHour.count());
        std::cout << "File already exists at " << filePath.c_str() << std::endl;
        std::vector<KrakenOHLC> ohlcPoints;
        saveHourOfTrades(floorHour, pair);
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
            ohlcPoints = saveHourOfTrades(floorHour, pair);
        }
        return ohlcPoints;
    }

    std::vector<KrakenOHLC> KrakenLoader::saveHourOfTrades(std::chrono::hours hour, const std::string&pair) {
        // reads trades for the next hour and saves them to a file
        std::chrono::seconds startSeconds = std::chrono::duration_cast<std::chrono::seconds>(hour);
        std::vector<KrakenTrade> trades = client.getTrades(startSeconds.count(), pair);
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
            nlohmann::json j_vec;
            std::ranges::for_each(ohlcPoints, [&](const KrakenOHLC& ohlc) {
                const nlohmann::json json_obj = ohlc;
                j_vec.push_back(json_obj);
            });
            std::cout << "File created at path: " << pairPath.c_str() << std::endl;
            // Optionally, you can write to the file here if needed.
            file << std::setw(4) << j_vec << std::endl;
            file.close();
        }
        return ohlcPoints;
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
                if (count != 0) {
                    average.setVwap(average.getVwap() / count);
                    average.setCount(count);
                    average.setTime(std::chrono::duration_cast<std::chrono::seconds>(nextMinute).count());
                    result.push_back(average);
                }
                nextMinute += std::chrono::minutes(1);
                average = KrakenOHLC();
                count = 0;
            }
        });
        return result;
    }
}