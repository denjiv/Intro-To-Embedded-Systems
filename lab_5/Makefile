# Makefile
# Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
# 2 / Jun / 2017
#
# Makefile for Lab 5 source codes

CC=gcc -std=gnu99 -g -Wall -lrt

all: motor uart_drive uart_drive_pi pi_uart

motor: motor.c motor.h
	$(CC) -o motor motor.c

uart_drive: uart_drive.c motor.h
	$(CC) -o uart_drive uart_drive.c

uart_drive_pi: uart_drive_pi.c motor.h
	$(CC) -o uart_drive_pi uart_drive_pi.c

pi_uart: pi_uart.c
	$(CC) -o pi_uart pi_uart.c

.PHONY: clean

clean:
	rm -f *.o *~ motor uart_drive uart_drive_pi pi_uart
