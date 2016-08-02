cmake_minimum_required(VERSION 2.6)
project(orchestrack)

add_executable(orchestrack src/ui/main.cpp)

install(TARGETS orchestrack RUNTIME DESTINATION bin)