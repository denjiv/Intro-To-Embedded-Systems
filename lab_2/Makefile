# Makefile
# Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
# 10 / Apr / 2017
#
# Makefile for Lab 2 source codes

CC=gcc -std=c1x -g -Wall

all: play_game read_fifo write_fifo jitter

read_fifo: read_fifo.c master.h
	$(CC) -o read_fifo read_fifo.c

write_fifo: write_fifo.c master.h
	$(CC) -o write_fifo write_fifo.c

play_game: play_game.c master.h Hangman.c Hangman.h
	$(CC) -o play_game play_game.c Hangman.c

jitter: jitter.c
	$(CC) -o jitter jitter.c

.PHONY: clean

clean:
	rm -f *.o *~ write_fifo read_fifo play_game jitter
