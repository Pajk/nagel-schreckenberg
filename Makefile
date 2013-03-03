# Makefile : Compile cellular.cc

SOURCES=car.cc track.cc nash.cc

all:	${SOURCES}
	g++ -O4 -o nash ${SOURCES} -lga

gui: ${SOURCES}
	g++ -O4 -o gui-nash ${SOURCES} -lga `allegro-config --libs` -DGUI

install: all gui
	cp nash /usr/bin
	cp gui-nash /usr/bin

uninstall:
	rm -f /usr/bin/nash /usr/bin/gui-nash

distrib:
	tar -c nash.cc car.cc car.h track.cc track.h Makefile > xpokor12.tar; \
	gzip xpokor12.tar

clean:
	rm -f nash gui-nash
