cmake_minimum_required(VERSION 2.6)
project(orchestrack)
if (APPLE)
  include_directories(/usr/local/include)
endif (APPLE)
add_executable(orchestrack src/ui/main.cpp)

install(TARGETS orchestrack RUNTIME DESTINATION bin)
