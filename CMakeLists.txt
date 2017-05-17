cmake_minimum_required(VERSION 3.5)
project(PROJEKT_SO)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lncurses -O0")

set(SOURCE_FILES main.cpp gui/ScreenManager.cpp gui/ScreenManager.h util/Point.h config/GlobalConfig.h core/CrawlerState.cpp core/CrawlerState.h core/ThreadPool.h)
add_executable(PROJEKT_SO ${SOURCE_FILES})