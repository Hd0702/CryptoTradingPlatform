#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "../src/encryption/EncryptionHelper.hpp"
TEST(Foo, Bar) {
    auto csafd = Encryption::B64Encode("stadsadsa");
    ASSERT_THAT(0, ::testing::Eq(0));
}