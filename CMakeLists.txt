cmake_minimum_required(VERSION 3.20)
project(btc-stats LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Find and link Boost libraries
find_package(Boost REQUIRED COMPONENTS
    program_options
    filesystem
    system
    json
)

add_executable(btc-stats
    src/main.cpp
    src/blockheader.cpp
    src/file.cpp
)

target_include_directories(btc-stats PRIVATE include ${Boost_INCLUDE_DIRS})
target_include_directories(btc-stats PRIVATE external/cppcodec)
target_link_libraries(btc-stats PRIVATE
    Boost::program_options
    Boost::filesystem 
    Boost::system 
    Boost::json
)

