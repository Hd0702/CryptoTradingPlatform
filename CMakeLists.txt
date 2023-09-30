cmake_minimum_required(VERSION 3.26)
project(CryptoTradingPlatform)
set(CMAKE_CXX_STANDARD 20)

# Boost
find_package(Boost 1.81 COMPONENTS program_options REQUIRED)
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
#Jsoncpp
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

include_directories(src)

add_library(PlatformLib src/main.cpp src/exchanges/kraken/KrakenClient.hpp src/exchanges/kraken/KrakenClient.cpp src/exchanges/kraken/KrakenClient.cpp src/config/EnvReader.hpp src/config/EnvReader.cpp src/encryption/EncryptionHelper.hpp src/encryption/EncryptionHelper.cpp
        src/config/EnvReader.cpp)
include(CTest)
enable_testing()
add_subdirectory(tst)

add_executable(CryptoTradingPlatform src/main.cpp src/exchanges/kraken/KrakenClient.hpp src/exchanges/kraken/KrakenClient.cpp src/exchanges/kraken/KrakenClient.cpp src/config/EnvReader.hpp src/config/EnvReader.cpp src/encryption/EncryptionHelper.hpp src/encryption/EncryptionHelper.cpp src/exchanges/coinbase/CoinbaseClient.hpp src/time/Clock.hpp src/time/Clock.cpp src/exchanges/coinbase/CoinbaseClient.cpp
        src/config/EnvReader.cpp)
TARGET_LINK_LIBRARIES(CryptoTradingPlatform  LINK_PUBLIC ${Boost_LIBRARIES} LINK_PUBLIC ${CURL_LIBRARIES} LINK_PUBLIC ${OPENSSL_LIBRARIES} LINK_PUBLIC ${JSONCPP_LINK_LIBRARIES})