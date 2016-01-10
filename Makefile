
CXX = g++
CXXFLAGS = -fPIC -std=c++11 -Wall -I/usr/local/include/pbc -I/usr/include/glib-2.0 \
		   -I/usr/lib/x86_64-linux-gnu/glib-2.0/include 

LDFLAGS = -lglib-2.0 \
		  -lgmp \
		  -lcrypto \
		  -lpython2.7 \
		  -lpbc \
		  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions

.PHONY:clean


all: abe.so test



abe.so: libbswabe.a abe.cc common.o
	$(CXX) -o abe.so abe.cc common.o libbswabe.a $(CXXFLAGS) $(LDFLAGS) -shared

common.o: common.c common.h
	$(CXX) -O1 -c common.c $(CXXFLAGS)

test: libbswabe.a abe.cc common.o
	$(CXX) abe.cc common.o libbswabe.a $(CXXFLAGS) $(LDFLAGS) -g -o test

clean:
	rm -rf *.out *.o *.so test

