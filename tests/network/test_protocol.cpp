#include <gtest/gtest.h>
// #include "network/Protocol.hpp"

// TODO: Implémenter les tests quand le protocole sera créé

// Test de sérialisation de paquet
// TEST(ProtocolTest, SerializePlayerInput) {
//     PlayerInputPacket packet;
//     packet.header.type = PacketType::PLAYER_INPUT;
//     packet.header.sequence = 42;
//     packet.playerId = 1;
//     packet.inputFlags = 0b00001111;  // UP, DOWN, LEFT, RIGHT
//
//     // Sérialiser et vérifier la taille
//     auto data = serialize(packet);
//     EXPECT_GT(data.size(), 0);
// }

// Test de désérialisation de paquet
// TEST(ProtocolTest, DeserializePlayerInput) {
//     PlayerInputPacket original;
//     original.header.type = PacketType::PLAYER_INPUT;
//     original.header.sequence = 42;
//     original.playerId = 1;
//     original.inputFlags = 0b00001111;
//
//     auto data = serialize(original);
//     auto deserialized = deserialize<PlayerInputPacket>(data);
//
//     EXPECT_EQ(deserialized.header.type, PacketType::PLAYER_INPUT);
//     EXPECT_EQ(deserialized.header.sequence, 42);
//     EXPECT_EQ(deserialized.playerId, 1);
//     EXPECT_EQ(deserialized.inputFlags, 0b00001111);
// }

// Test de taille de paquet
// TEST(ProtocolTest, PacketSizes) {
//     EXPECT_EQ(sizeof(PacketHeader), 7);  // 1 + 4 + 2 bytes
//     EXPECT_LT(sizeof(PlayerInputPacket), 256);  // Moins de 256 bytes
// }

// Placeholder test
TEST(ProtocolTest, Placeholder) {
    EXPECT_TRUE(true);
}
