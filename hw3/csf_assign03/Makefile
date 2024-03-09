# Makefile for CSF Assignment 3

CC = g++
CXXFLAGS = -g -Wall -Wextra -pedantic -std=c++17

csim: main.o functions.o
	$(CC) $(CXXFLAGS) -o csim main.o functions.o

main.o: main.cpp functions.h
	$(CC) $(CXXFLAGS) -c main.cpp

functions.o: functions.cpp functions.h
	$(CC) $(CXXFLAGS) -c functions.cpp

clean:
	rm -f *.o csim