# This CMakeLists file is for usage with CLion (and maybe other) IDEs ONLY.
# Do NOT attempt to build the project with CMake as it will fail
# (kernel build process is tailored for Makefile while CLion's support for Makefile is... meh)
# idea stolen from https://github.com/RedPill-TTG/redpill-lkm/

cmake_minimum_required(VERSION 3.0)
project(networkfs C)

include_directories(include)

# substitute 5.4.0-90-generic with $(uname -r) of your TARGET machine
include_directories(/usr/src/linux-headers-5.4.0-90-generic/include)
# you will also want to install these headers to your main machine
# (don't forget to choose the version that matches your target machine)

# if you're running ubuntu, you can do it with:
# $ sudo apt install linux-headers-VERSION
# if you're running other distro, you know your packet manager better than I do
# if you're running Windows, I'm sorry

add_executable(networkfs src/networkfs.c src/utils.c)
