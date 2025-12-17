##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt, CMake configuration for Test environment
##

# Find dependencies
find_package(GTest REQUIRED)
find_package(asio REQUIRED)

set(TEST_SOURCES
    network/test_protocol.cpp
    ecs/test_registry.cpp
    ecs/test_components.cpp
    logger/test_logger.cpp
)

# target_include_directories(air-trap-tests INTERFACE 
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

# add_executable(test_network network/test_protocol.cpp)
# 
# target_link_libraries(test_network 
#     PUBLIC 
#         RTypeCommon
#     PRIVATE
#         asio::asio
#         gtest::gtest 
# )

add_executable(test_ecs
    ecs/test_registry.cpp
    ecs/test_components.cpp
    ecs/test_sparsearray.cpp
    ecs/test_zipview.cpp
)

target_link_libraries(test_ecs 
    PUBLIC 
        RTypeCommon
    PRIVATE
        gtest::gtest
)

add_executable(test_logger logger/test_logger.cpp)

target_link_libraries(test_logger 
    PUBLIC 
        RTypeCommon
    PRIVATE
        gtest::gtest
)

include(GoogleTest)
# gtest_discover_tests(test_network)
gtest_discover_tests(test_ecs)
gtest_discover_tests(test_logger)