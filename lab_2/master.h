/* master.h
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 5 / Apr / 2017
 *
 * Header file for LCD driver (write_fifo and read_fifo)
 */

#ifndef MASTER_H
#define MASTER_H

#include <stdio.h>      // for File IO and printf
#include <unistd.h>       // for usleep
#include <stdint.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define GPIO_PIN_COUNT 11
#define DB0_PIN "67"
#define DB1_PIN "68"
#define DB2_PIN "44"
#define DB3_PIN "26"
#define DB4_PIN "46"
#define DB5_PIN "65"
#define DB6_PIN "61"
#define DB7_PIN "66"
#define RW_PIN "69"
#define RS_PIN "45"
#define EN_PIN "47"

#define DB0 0
#define DB1 1
#define DB2 2
#define DB3 3
#define DB4 4
#define DB5 5
#define DB6 6
#define DB7 7
#define RW 8
#define RS 9
#define EN 10

#define MAX_BUF 81
#define MASK_WRITE 0x0001
#define MASK_LCD_WRITE 0x0200

FILE* lcd_pins[GPIO_PIN_COUNT];
const char* myfifo = "/tmp/myfifo";

// Returns the size of the string
int size_str(char* str);

// Writes to the fifo pipe
void write_fifo(char* str);

// Flushed the stdin; removes the oveflow user input (> 32 chars)
void flush_stdin();

#endif
