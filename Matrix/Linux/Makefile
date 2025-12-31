CXX = g++
CXXFLAGS = -std=c++17 -O3 -pthread

TARGETS = matrix_thread matrix_pthread

all: $(TARGETS)

matrix_thread: stream_thread.cpp
	$(CXX) $(CXXFLAGS) stream_thread.cpp -o matrix_thread

matrix_pthread: stream_pthread.cpp
	$(CXX) $(CXXFLAGS) stream_pthread.cpp -o matrix_pthread

clean:
	rm -f $(TARGETS)