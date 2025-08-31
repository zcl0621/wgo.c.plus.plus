cmake_minimum_required(VERSION 3.14)

set(CMAKE_TOOLCHAIN_FILE "$ENV{HOME}/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project(consoleGo VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Find GTest package
find_package(GTest REQUIRED)

# Enable testing
enable_testing()

# Include directories
include_directories(
    ${GTEST_INCLUDE_DIRS}
)

# Main executable
add_executable(consoleGo
    main.cpp
        io.h
        board.h
        utils.h
        node.h
        colour.h
)

add_executable(GameTests
    test/test_game.cpp
)

# Link test executable with GTest
target_link_libraries(GameTests
    GTest::GTest
        GTest::Main
        pthread
)

# Add test
add_test(NAME GameTests COMMAND GameTests)
