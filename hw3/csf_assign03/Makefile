# Makefile for CSF Assignment 3

CC = g++
CXXFLAGS = -g -Wall -std=c++17

csim: main.o
	$(CC) $(CXXFLAGS) -o csim main.o

main.o: main.cpp
	$(CC) $(CXXFLAGS) -c main.cpp

clean:
	rm -f *.o csim