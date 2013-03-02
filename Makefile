# Makefile : Compile cellular.cc

all:	car.cc track.cc nash.cc
	g++ -O4 -o nash car.cc track.cc nash.cc -lga `allegro-config --libs`
clean:
	rm -f nash
