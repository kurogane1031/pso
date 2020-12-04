cmake_minimum_required(VERSION 3.10)
project(PSO VERSION 0.1.5)

find_package(FMT)
find_package(range-v3 CONFIG REQUIRED)

SET(CMAKE_CXX_COMPILER "/usr/bin/clang++-10")
SET(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -Wpedantic -ftime-trace")
SET(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

add_executable(pso pso.cpp main.cpp)
target_link_libraries(pso fmt)

# CXX = clang++-10
# CXXFLAGS = -std=c++14 -g -O3 -ftime-trace
# LDFLAGS = -lpython3.6m 
# IFLAGS = -I/usr/include/eigen3/ -I/usr/include/python3.6m

# main.o: main.cpp 
# $(CXX) $(CXXFLAGS) -o $@ $^ $(IFLAGS) $(LDFLAGS)

# test_main.o: test.cpp
# $(CXX) $(CXXFLAGS) -o $@ $^ $(IFLAGS) $(LDFLAGS) -lgtest -lgtest_main -pthread

# string_example.o: example.cpp
# $(CXX) $(CXXFLAGS) -o $@ $^ 

# clean:
# rm -f *.o
