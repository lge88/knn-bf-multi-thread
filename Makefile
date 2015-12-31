CC=g++
CPPFLAGS=-std=c++0x -O3

knn.bf: knn.bf.cpp
	$(CC) $(CPPFLAGS) $^ -o $@
