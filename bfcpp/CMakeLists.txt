# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (bfcpp C CXX)

set(CMAKE_BUILD_TYPE "Debug")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_NANOSLEEP")

# Include sub-projects.
add_subdirectory("bfcpptest")
add_subdirectory("bfcpplib")


