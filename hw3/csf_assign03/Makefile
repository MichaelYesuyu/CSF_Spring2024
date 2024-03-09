# Makefile for CSF Assignment 3

CC = g++
CXXFLAGS = -g -Wall -Wextra -pedantic -std=c++17

csim: main.o cacheSimulator.o
	$(CC) $(CXXFLAGS) -o csim main.o cacheSimulator.o

main.o: main.cpp cacheSimulator.h
	$(CC) $(CXXFLAGS) -c main.cpp

cacheSimulator.o: cacheSimulator.cpp cacheSimulator.h
	$(CC) $(CXXFLAGS) -c cacheSimulator.cpp

clean:
	rm -f *.o csim