/* motor.h
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 3 / May / 2017
 *
 * Header file for motor.c
 */

#ifndef MOTOR_H
#define MOTOR_H

#include <time.h>       // for usleep
#include <stdio.h>      // for File IO and printf
#include <unistd.h>     // for usleep
#include <stdint.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <math.h>

#define GPIO_PIN_COUNT 4
#define AIN1 "67"
#define AIN2 "68"
#define BIN2 "44"
#define BIN1 "26"

#define PWM_PIN_COUNT 2
#define PWMA "bone_pwm_P8_13"
#define PWMB "bone_pwm_P8_19"

#define MAX_BUF 81
#define THRESHOLD 1000
#define FREQ 50000 //Hz

const char* myfifo = "/tmp/myfifo";

FILE *ain1, *ain2, *bin2, *bin1;
char buf[MAX_BUF];
timer_t timerID;
struct itimerspec spec;
int autoT;

#endif
