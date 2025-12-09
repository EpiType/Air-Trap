cmake_minimum_required(VERSION 3.20)

# Configuration des tests
find_package(GTest REQUIRED)

# Tests ECS
add_executable(test_ecs
    ecs/test_registry.cpp
    ecs/test_components.cpp
)

target_link_libraries(test_ecs
    PRIVATE
        RTypeCommon
        GTest::gtest
        GTest::gtest_main
)

target_include_directories(test_ecs
    PRIVATE
        ${CMAKE_SOURCE_DIR}/common/include
)

# Tests Network
add_executable(test_network
    network/test_protocol.cpp
)

target_link_libraries(test_network
    PRIVATE
        RTypeCommon
        GTest::gtest
        GTest::gtest_main
)

target_include_directories(test_network
    PRIVATE
        ${CMAKE_SOURCE_DIR}/common/include
)

# Activer les tests
enable_testing()
add_test(NAME ECS_Tests COMMAND test_ecs)
add_test(NAME Network_Tests COMMAND test_network)
