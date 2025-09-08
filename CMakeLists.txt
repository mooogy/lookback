cmake_minimum_required(VERSION 4.1)
project(lookback VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)


# if(ENABLE_PROFILING)
#     message(STATUS "Profiling enabled")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -g")
#     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
# else()
#     message(STATUS "Profiling disabled")
# endif()

add_library(lookback INTERFACE)
target_include_directories(lookback INTERFACE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

add_library(lookback::lookback ALIAS lookback)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE lookback::lookback)