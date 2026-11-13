# CPU Scheduling Simulator - CMake (Visual Studio "Klasor Ac", CLion, VS Code, Linux/macOS)
cmake_minimum_required(VERSION 3.10)
project(CPUSchedulingSimulator CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SIM_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
set(LIB_SOURCES ${SIM_SOURCES})
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

add_executable(scheduler ${SIM_SOURCES})
target_include_directories(scheduler PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(run_tests ${LIB_SOURCES} ${CMAKE_SOURCE_DIR}/tests/test_scheduler.cpp)
target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

foreach(tgt scheduler run_tests)
  if(MSVC)
    target_compile_options(${tgt} PRIVATE /W4)
  else()
    target_compile_options(${tgt} PRIVATE -Wall -Wextra)
  endif()
endforeach()

enable_testing()
add_test(NAME unit_tests COMMAND run_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
