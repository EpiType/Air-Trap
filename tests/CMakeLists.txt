##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt, CMake configuration for Test environment
##

set(TEST_SOURCES
    network/test_protocol.cpp
    ecs/test_registry.cpp
)

# target_include_directories(air-trap-tests INTERFACE 
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

add_executable(test_network network/test_protocol.cpp)

target_link_libraries(test_network 
    PRIVATE 
        RTypeCommon
        asio::asio
        gtest::gtest 
)

add_executable(test_ecs ecs/test_registry.cpp)

target_link_libraries(test_ecs 
    PRIVATE 
        RTypeCommon
        gtest::gtest
)

include(GoogleTest)
gtest_discover_tests(test_network DISCOVERY_MODE PRE_BUILD)
gtest_discover_tests(test_ecs DISCOVERY_MODE PRE_BUILD)