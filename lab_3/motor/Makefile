# Makefile
# Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
# 1 / May / 2017
#
# Makefile for Lab 3&4 source codes

CC=gcc -std=c1x -g -Wall

all: motor uart_drive

motor: motor.c motor.h
	$(CC) -o motor motor.c

uart_drive: uart_drive.c uart_drive.h
	$(CC) -o uart_drive uart_drive.c

.PHONY: clean

clean:
	rm -f *.o *~ motor uart_drive
