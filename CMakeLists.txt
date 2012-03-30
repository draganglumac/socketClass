project(sleeper)
cmake_minimum_required(VERSION 2.8)
find_package(Boost COMPONENTS thread REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
add_executable(sleeper src/socketcontrol.cpp main.cpp)

target_link_libraries(sleeper ${Boost_THREAD_LIBRARY})
add_subdirectory(src)