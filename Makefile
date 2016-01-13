
CC = gcc
CXX = g++
CFLAGS = -fPIC -Wall -I/usr/local/include/pbc -I/usr/include/glib-2.0 \
		 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include 
CXXFLAGS = -fPIC -std=c++11 -Wall -I/usr/local/include/pbc -I/usr/include/glib-2.0 \
		   -I/usr/lib/x86_64-linux-gnu/glib-2.0/include 

LDFLAGS = -lglib-2.0 \
		  -lgmp \
		  -lcrypto \
		  -lpython2.7 \
		  -lpbc \
		  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions

.PHONY:clean


all: abe.so



abe.so: libbswabe.a abe.o common.o policy_lang.o
	$(CXX) -O2 -o abe.so abe.o common.o policy_lang.o libbswabe.a $(CXXFLAGS) $(LDFLAGS) -shared

abe.o: abe.cc common.h policy_lang.h private.h
	$(CXX) -O2 -c abe.cc $(CXXFLAGS)

common.o: common.c common.h
	$(CXX) -O2 -c common.c $(CXXFLAGS)

policy_lang.o: policy_lang.c policy_lang.h common.h
	$(CC) -O2 -c policy_lang.c $(CFLAGS)


clean:
	rm -rf *.out *.o *.so test

