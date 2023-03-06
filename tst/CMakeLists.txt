set(BINARY ${CMAKE_PROJECT_NAME}_tst)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

set(TEST_SRCS ${TEST_SOURCES})

include_directories(${CrytoTradingPlatformSrc_SOURCE_DIR})
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.13.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
add_executable(${BINARY} ${TEST_SOURCES} encryption/EncryptionHelperTest.cpp)

add_test(NAME ${BINARY} COMMAND ${BINARY})
target_include_directories(${BINARY} PRIVATE "${GMOCK_INCLUDE_DIRS}" "${GTEST_INCLUDE_DIRS}")
target_link_libraries(${BINARY} PUBLIC gtest gtest_main gmock_main ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${OPENSSL_LIBRARIES} PlatformLib)
include(GoogleTest)