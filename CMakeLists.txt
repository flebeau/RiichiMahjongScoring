cmake_minimum_required(VERSION 3.1.0)

project(MahjongScoring)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

file(GLOB SRCS src/*.cpp)
add_executable(MahjongScoring ${SRCS})
target_link_libraries(MahjongScoring Qt5::Widgets)