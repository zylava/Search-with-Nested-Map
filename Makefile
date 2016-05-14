CXX=g++
CXXFLAGS = -std=c++0x -I/usr/local/include
GENERATED = 2014-10-31

all: query.sh generate.sh

query.sh: 
	$(CXX) -o query.sh $^ $(CXXFLAGS) query.cpp

generate.sh: 
	$(CXX) -o generate.sh $^ $(CXXFLAGS) generate.cpp

clean:
	rm -rf *.o *~ $(GENERATED) *.sh