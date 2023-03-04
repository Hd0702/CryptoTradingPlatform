cmake_minimum_required(VERSION 3.24)
project(CryptoTradingProject)
include(FetchContent)

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

add_executable(CryptoTradingPlatform src/main.cpp src/exchanges/KrakenClient.h src/exchanges/KrakenClient.cpp src/exchanges/KrakenClient.cpp src/config/EnvReader.h src/config/EnvReader.cpp)
TARGET_LINK_LIBRARIES(CryptoTradingPlatform  LINK_PUBLIC ${Boost_LIBRARIES} LINK_PUBLIC ${CURL_LIBRARIES} LINK_PUBLIC ${OPENSSL_LIBRARIES})
