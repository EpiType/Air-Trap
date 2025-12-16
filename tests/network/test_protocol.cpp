#include <gtest/gtest.h>
#include "RType/Network/Packet.hpp"

using namespace rtp::net;

TEST(PacketTest, DefaultConstruction) {
    Packet packet;
    EXPECT_TRUE(true); // Basic compilation test
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
