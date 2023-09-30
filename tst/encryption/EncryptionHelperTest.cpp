#include <gmock/gmock-matchers.h>
#include "gtest/gtest.h"
#include "encryption/EncryptionHelper.hpp"

struct Base64TestParameters : public testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(Base64TestParameters, Base64Test) {
    const std::string input = std::get<0>(Base64TestParameters::GetParam());
    std::string expectedEncodedOutput = std::get<1>(Base64TestParameters::GetParam());
    const auto actualEncodedOutput = Encryption::b64Encode(input);
    ASSERT_THAT(actualEncodedOutput.rfind(expectedEncodedOutput), ::testing::Eq(0));
    ASSERT_THAT(input, Encryption::b64Decode(Encryption::b64Encode(input)));
}

INSTANTIATE_TEST_SUITE_P(
        Base64Test,
        Base64TestParameters,
        testing::Values(
                std::make_pair("abcd", "YWJjZA"),
                std::make_pair("SPECIAL_CHARACTER_EXAMPLE", "U1BFQ0lBTF9DSEFSQUNURVJfRVhBTVBMRQ")
                )
        );
