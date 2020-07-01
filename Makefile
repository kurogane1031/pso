CXX = g++
CXXFLAGS = -std=c++14 -g -O3
LDFLAGS = -lpython3.6m 
IFLAGS = -I/usr/include/eigen3/ -I/usr/include/python3.6m

main.o: main.cpp pso.h
	$(CXX) $(CXXFLAGS) -o $@ $^ $(IFLAGS) $(LDFLAGS)

test_main.o: test.cpp pso.h
	$(CXX) $(CXXFLAGS) -o $@ $^ $(IFLAGS) $(LDFLAGS) -lgtest -lgtest_main -pthread

clean:
	rm -f *.o
