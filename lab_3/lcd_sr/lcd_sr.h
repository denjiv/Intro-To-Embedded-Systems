/* lcd_sr.h
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 3 / May / 2017
 *
 * Header file for lcd_sr.c
 */

#ifndef LCD_SR_H
#define LCD_SR_H

#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

#define GPIO_PIN_COUNT 5
#define SR_CLK_PIN 48
#define ST_CLK_PIN 60
#define SER_PIN 49
#define RS_PIN 20
#define EN_PIN 115

#define SR_CLK 0
#define ST_CLK 1
#define SER 2
#define RS 3
#define EN 4

#define MAX_BUF 81
#define MASK_WRITE 0x01

const int lcd_pins[GPIO_PIN_COUNT] = {SR_CLK_PIN, ST_CLK_PIN, SER_PIN, RS_PIN, EN_PIN};
const char* lcd_name[GPIO_PIN_COUNT] = {"LCD_SR_CLK", "LCD_ST_CLK", "LCD_SER", "LCD_RS", "LCD_EN"};

uint8_t cur_pos = 0; // current cursor position

void write_en(void);
void sr_clk_pulse(void);
void st_clk_pulse(void);
void write_lcd(uint8_t val);
void init_display(void);

#define DEVICE_NAME "lcd_sr_device"

/* data structures */
// contains data about the device.
// data : buffer for character data stored.
// sem  : semaphore.
struct fake_device {
	char data[100];
	struct semaphore sem;
} virtual_device;

/* global variables */
// stores info about this char device.
static struct cdev* mcdev;
// holds major and minor number granted by the kernel
static dev_t dev_num;

/* function prototypes */
// file operations
static int __init lcd_start(void);
static void __exit lcd_exit(void);
static int  device_open(struct inode*, struct file*);
static int device_close(struct inode*, struct file *);
static ssize_t device_read(struct file*, char*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char*, size_t, loff_t*);

/* operations usable by this file. */
static struct file_operations fops = {
   .owner = THIS_MODULE,
   .read = device_read,
   .write = device_write,
   .open = device_open,
   .release = device_close
};

#endif
