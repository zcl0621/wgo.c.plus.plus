#include <gtest/gtest.h>


class GameTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
