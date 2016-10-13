cmake_minimum_required(VERSION 2.6)
project(orchestrack)
set(HAVE_JACK TRUE)
if (APPLE)
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
endif (APPLE)
include_directories(lib/json/src)
if (CMAKE_GENERATOR MATCHES "Visual Studio")
  message(WARNING "building for Visual, huh? support is little.")
  if (NOT DEFINED SDL_PATH)
    message(FATAL_ERROR "please define SDL_PATH (-DSDL_PATH=...) to SDL2 library location, and try again.")
  endif (NOT DEFINED SDL_PATH)
  if (NOT DEFINED SNDFILE_PATH)
    message(FATAL_ERROR "please define SNDFILE_PATH (-DSNDFILE_PATH=...) to sndfile library location, and try again.")
  endif (NOT DEFINED SNDFILE_PATH)
  if (NOT DEFINED SDL_TTF_PATH)
    message(FATAL_ERROR "please define SDL_TTF_PATH (-DSDL_TTF_PATH=...) to SDL2_ttf library location, and try again.")
  endif (NOT DEFINED SDL_TTF_PATH)
  if (NOT DEFINED SDL_IPATH)
    message(FATAL_ERROR "please define SDL_IPATH (-DSDL_IPATH=...) to SDL2 header files location, and try again.")
  endif (NOT DEFINED SDL_IPATH)
  if (NOT DEFINED SNDFILE_IPATH)
    message(FATAL_ERROR "please define SNDFILE_IPATH (-DSNDFILE_IPATH=...) to sndfile header files location, and try again.")
  endif (NOT DEFINED SNDFILE_IPATH)
  if (NOT DEFINED SDL_TTF_IPATH)
    message(FATAL_ERROR "please define SDL_TTF_IPATH (-DSDL_TTF_IPATH=...) to SDL2_ttf header files location, and try again.")
  endif (NOT DEFINED SDL_TTF_IPATH)
  message(STATUS "SDL path: ${SDL_PATH}")
  message(STATUS "libsndfile path: ${SNDFILE_PATH}")
  message(STATUS "SDL_ttf path: ${SDL_TTF_PATH}")
  message(STATUS "SDL include path: ${SDL_IPATH}")
  message(STATUS "libsndfile include path: ${SNDFILE_IPATH}")
  message(STATUS "SDL_ttf include path: ${SDL_TTF_IPATH}")
  include_directories(${SDL_IPATH} ${SNDFILE_IPATH} ${SDL_TTF_IPATH})
  link_directories(${SDL_PATH} ${SNDFILE_PATH} ${SDL_TTF_PATH})
  set(HAVE_JACK FALSE)
else (CMAKE_GENERATOR MATCHES "Visual Studio")
  find_library(HAVE_JACK jack)
endif (CMAKE_GENERATOR MATCHES "Visual Studio")
add_library(gfx STATIC lib/gfx/SDL2_rotozoom.c lib/gfx/SDL2_gfxPrimitives.c)
add_library(sampler STATIC
  src/engine/lutgen.cpp
  src/file/fextra.c
  src/format/aiff.cpp
  src/format/wave.cpp
  src/format/iff.cpp
  src/ui/button.cpp
  src/ui/knob.cpp
  src/instr/instr.cpp
  src/font/font.cpp
  src/instr/sampler/audio.cpp
  src/instr/sampler/interp.cpp
  src/instr/sampler/envedit.cpp
  src/instr/sampler/grid.cpp
  src/instr/sampler/smpedit.cpp
  src/instr/sampler/state.cpp
  src/instr/sampler/mouse.cpp
  src/instr/sampler/resize.cpp
  src/instr/sampler/erase.cpp
  src/instr/sampler/error.cpp
  src/instr/sampler/dir.cpp
  src/instr/sampler/sload.cpp
  src/instr/sampler/ssel.cpp
  src/instr/sampler/list.cpp
  src/instr/sampler/sampler.cpp)
add_executable(orchestrack src/ui/application.cpp src/ui/main.cpp)
add_executable(sampler-sa src/instr/sampler/main.cpp)
add_executable(sampler-bench src/instr/sampler/bench.cpp)
target_link_libraries(orchestrack SDL2)
target_link_libraries(gfx SDL2)
target_link_libraries(sampler SDL2 SDL2_ttf gfx sndfile)
if (HAVE_JACK)
target_compile_definitions(sampler-sa PUBLIC HAVE_JACK)
  target_link_libraries(sampler-sa SDL2 SDL2_ttf gfx sampler sndfile jack)
else (HAVE_JACK)
  target_link_libraries(sampler-sa SDL2 SDL2_ttf gfx sampler sndfile)
endif (HAVE_JACK)
target_link_libraries(sampler-bench SDL2 SDL2_ttf gfx sampler sndfile)
install(TARGETS orchestrack RUNTIME DESTINATION bin)
