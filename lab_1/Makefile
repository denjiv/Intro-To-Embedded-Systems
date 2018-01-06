# Makefile
# Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
# 5 / Apr / 2017
#
# Makefile for masterpiece.c

CC=gcc -g -Wall
SOURCES= masterpiece.c
EXECUTABLE = masterpiece

masterpiece: masterpiece.c
	$(CC) -o $(EXECUTABLE) $(SOURCES)

.PHONY: clean

clean:
	rm -f *.o *~ $(EXECUTABLE)
