cmake_minimum_required(VERSION 3.26.4)
project(CryptoTradingPlatform)
set(CMAKE_CXX_STANDARD 20)

# Boost
find_package(Boost 1.81 COMPONENTS program_options REQUIRED)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost Not found")
endif()

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
#Curl
find_package (CURL REQUIRED)
include_directories (${CURL_INCLUDE_DIR})
list(APPEND LIBS ${CURL_LIBRARIES})
#Open SSL
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package (OpenSSL REQUIRED)
include_directories (${OPENSSL_INCLUDE_DIR})
list(APPEND LIBS ${OPENSSL_LIBRARIES})
#json
find_package(nlohmann_json 3.2.0 REQUIRED)

include_directories(src)

add_library(PlatformLib src/main.cpp src/exchanges/kraken/KrakenClient.hpp src/exchanges/kraken/KrakenClient.cpp src/exchanges/kraken/KrakenClient.cpp src/config/EnvReader.hpp src/config/EnvReader.cpp src/encryption/EncryptionHelper.hpp src/encryption/EncryptionHelper.cpp
        src/config/EnvReader.cpp
        src/strategy/BaseStrategy.hpp
        src/strategy/BuyAndHold.hpp
)
include(CTest)
enable_testing()
add_subdirectory(tst)

add_executable(CryptoTradingPlatform src/main.cpp src/exchanges/kraken/KrakenClient.hpp src/exchanges/kraken/KrakenClient.cpp src/exchanges/kraken/KrakenClient.cpp src/config/EnvReader.hpp src/config/EnvReader.cpp src/encryption/EncryptionHelper.hpp src/encryption/EncryptionHelper.cpp src/exchanges/coinbase/CoinbaseClient.hpp src/time/Clock.hpp src/time/Clock.cpp src/exchanges/coinbase/CoinbaseClient.cpp
        src/config/EnvReader.cpp
        src/strategy/BaseStrategy.hpp
        src/strategy/BuyAndHold.hpp
        src/exchanges/kraken/KrakenOHLC.hpp
        src/exchanges/kraken/KrakenOHLC.cpp
        src/exchanges/BaseExchange.hpp
        src/exchanges/BaseExchange.hpp
        src/exchanges/BaseExchange.hpp
        src/exchanges/BaseOHLC.hpp
        src/strategy/BuyAndHold.cpp
        src/TradeLineItem.hpp
        src/exchanges/kraken/KrakenLoader.hpp
        src/exchanges/kraken/KrakenLoader.cpp
        src/exchanges/kraken/KrakenTrade.hpp
        src/strategy/moving_average/MovingAverageCrossover.cpp
        src/strategy/moving_average/MovingAverageCrossover.hpp
        src/strategy/moving_average/MovingAverageTrade.hpp
        src/exchanges/kraken/KrakenTradeInfo.hpp
        src/time/time_concepts.hpp
)
TARGET_LINK_LIBRARIES(CryptoTradingPlatform LINK_PUBLIC Boost::program_options LINK_PUBLIC ${CURL_LIBRARIES} LINK_PUBLIC ${OPENSSL_LIBRARIES} LINK_PUBLIC nlohmann_json::nlohmann_json)