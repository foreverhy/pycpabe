
CXX = g++
CXXFLAGS = -fPIC -std=c++11 -Wall -I/usr/local/include/pbc -I/usr/include/glib-2.0 \
		   -I/usr/lib/x86_64-linux-gnu/glib-2.0/include 

LDFLAGS = -lglib-2.0 \
		  -lgmp \
		  -lcrypto \
		  -lpython2.7 \
		  -lpbc 

all: abe.so test



abe.so: libbswabe.a abe.cc
	$(CXX) -O2 abe.cc -o abe.so libbswabe.a -shared $(CXXFLAGS) $(LDFLAGS)

test: libbswabe.a abe.cc
	$(CXX) abe.cc libbswabe.a $(CXXFLAGS) $(LDFLAGS) -g -o test

clean:
	rm -rf *.out *.o *.so test

