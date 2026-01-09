CXX = g++
CXXFLAGS = -std=c++11 -fPIC -Wall

STATIC_LIB = libNumber.a
DYNAMIC_LIB = libVector.so
EXE = ExampleApp

all: $(EXE)

$(STATIC_LIB): Number.cpp Number.h
	$(CXX) $(CXXFLAGS) -c Number.cpp -o Number.o
	ar rcs $(STATIC_LIB) Number.o

$(DYNAMIC_LIB): Vector.cpp Vector.h $(STATIC_LIB)
	$(CXX) $(CXXFLAGS) -DVECTOR_EXPORTS -shared Vector.cpp -o $(DYNAMIC_LIB) -L. -lNumber

$(EXE): Example.cpp $(STATIC_LIB) $(DYNAMIC_LIB)
	$(CXX) $(CXXFLAGS) Example.cpp -o $(EXE) -L. -lVector -lNumber -Wl,-rpath,.

clean:
	rm -f *.o *.a *.so $(EXE)
