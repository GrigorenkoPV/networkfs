# This CMakeLists file is for usage with CLion (and maybe other) IDEs ONLY.
# Do NOT attempt to build the project with CMake as it will fail
# (kernel build process is tailored for Makefile while CLion's support for Makefile is... meh)
# stolen from https://github.com/RedPill-TTG/redpill-lkm/

cmake_minimum_required(VERSION 3.0)
project(networkfs C)

include_directories(include)
include_directories(/usr/src/linux-headers-5.4.0-90-generic/include)

add_executable(networkfs src/networkfs.c src/utils.c)
