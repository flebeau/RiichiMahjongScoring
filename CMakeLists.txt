cmake_minimum_required(VERSION 3.1.0)

project(RiichiMahjongScoring)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -Wall")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

file(GLOB SRCS src/*.cpp)
add_executable(RiichiMahjongScoring ${SRCS})
target_link_libraries(RiichiMahjongScoring Qt5::Widgets)
