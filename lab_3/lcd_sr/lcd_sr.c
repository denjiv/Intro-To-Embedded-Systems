/* lcd_sr.c
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 3 / May / 2017
 *
 * Kernel module for LCD.
 */

#include "lcd_sr.h"

static int __init lcd_start(void) {
    int i;
    int ret;
	printk(KERN_INFO "LCD is now booting!\n");

    for (i = 0; i < GPIO_PIN_COUNT; i++) {
        gpio_request(lcd_pins[i], lcd_name[i]);
        gpio_direction_output(lcd_pins[i], 0);
        gpio_export(lcd_pins[i], true);
        printk(KERN_INFO "LCD pin %d\n", lcd_pins[i]);
    }

    gpio_set_value(lcd_pins[RS], 0); // keep RS low for init
    msleep(1);

    init_display();
    printk(KERN_INFO "LCD is now intialized!\n");

    // REGISTERIONG OUR DEVICE WITH THE SYSTEM
    // (1) ALLOCATE DINAMICALLY TO ASSIGN OUR DEVICE
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		printk(KERN_ALERT "lcd_device: Failed to allocate a major number\n");
		return ret;
	}
	printk(KERN_INFO "lcd_device: major number is %d\n", MAJOR(dev_num));
	printk(KERN_INFO "Use mknod /dev/%s c %d 0 for device file\n", DEVICE_NAME, MAJOR(dev_num));

	// (2) CREATE CDEV STRUCTURE, INITIALIZING CDEV
	mcdev = cdev_alloc();
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;

	// After creating cdev, add it to kernel
	ret = cdev_add(mcdev, dev_num, 1);
	if (ret < 0) {
		printk(KERN_ALERT "lcd_device: unable to add cdev to kernerl\n");
		return ret;
	}
	// Initialize SEMAPHORE
	sema_init(&virtual_device.sem, 1);
	return 0;
}

// Exit the device
static void __exit lcd_exit(void) {
    cdev_del(mcdev);
    unregister_chrdev_region(dev_num, 1);
	printk(KERN_INFO "LCD is done!\n");
}

// Toggle the enable pin
void write_en(void) {
    gpio_set_value(lcd_pins[EN], 0);
	udelay(1);
	gpio_set_value(lcd_pins[EN], 1);
	udelay(1);
	gpio_set_value(lcd_pins[EN], 0);
	udelay(1);
}

// Shift register clock toggle
void sr_clk_pulse(void) {
	gpio_set_value(lcd_pins[SR_CLK], 0);
	udelay(1);
	gpio_set_value(lcd_pins[SR_CLK], 1);
	udelay(1);
	gpio_set_value(lcd_pins[SR_CLK], 0);
	udelay(1);
}

// Storage register clock toggle
void st_clk_pulse(void) {
	gpio_set_value(lcd_pins[ST_CLK], 0);
	udelay(1);
	gpio_set_value(lcd_pins[ST_CLK], 1);
	udelay(1);
	gpio_set_value(lcd_pins[ST_CLK], 0);
	udelay(1);
}

// Write to the lcd display based on pin values in val (shift reg)
void write_lcd(uint8_t val) {
	uint8_t i;
    uint8_t pin_out;
    uint8_t mask = 128;
	msleep(1);
	for(i = 0; i < 8; i++) {
        pin_out = val & mask;
        if (pin_out == 0) {
        	pin_out = 0;
        } else {
            pin_out = 1;
        }
        gpio_set_value(lcd_pins[SER], pin_out);
		mask = mask >> 1;
		sr_clk_pulse();
	}
	st_clk_pulse(); // push finalized output out of storage register
	write_en(); // allow LCD to pick up data from stortge register
}

// Initialize the display
void init_display(void) {
    msleep(20);
	write_lcd(0x30);
	msleep(5);
	write_lcd(0x30);
	msleep(1);
	write_lcd(0x30);
	msleep(1);

	write_lcd(0x38); // function set
	write_lcd(0x08); // Display OFF
	write_lcd(0x01); // Clear Display
	msleep(1);
	write_lcd(0x06); // Entry Mode
	write_lcd(0x0F); // Display ON
	// Initialization Complete

    msleep(1);
	gpio_set_value(lcd_pins[RS], 1); // pull RS high for all future writing operations
}

// Called on device file open
// inode reference to file on disk, struct file represents an abstract
// checks to see if file is already open (semaphore is in use)
// prints error message if device is busy.
int device_open(struct inode *inode, struct file* filp) {
	if (down_interruptible(&virtual_device.sem) != 0) {
		printk(KERN_ALERT "lcd_device: could not lock device during open\n");
		return -1;
	}
	printk(KERN_INFO "lcd_device: device opened\n");
	return 0;
}

// Called upon close
// closes device and returns access to semaphore.
int device_close(struct inode* inode, struct  file *filp) {
	up(&virtual_device.sem);
	printk(KERN_INFO "lcd_device, closing device\n");
	return 0;
}

// Called when user wants to get info from device file
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset) {
	printk(KERN_INFO "lcd_device: Reading from device...\n");
	return copy_to_user(bufStoreData, virtual_device.data, bufCount);
}

// Called when user wants to send info to device
ssize_t device_write(struct file* filp, const char* bufSource, size_t bufCount, loff_t* curOffset) {
    uint8_t i = 0;
    ssize_t rtn = copy_from_user(virtual_device.data, bufSource, bufCount);
    printk(KERN_INFO "bufSource: %s\n", bufSource);
    msleep(1);

    while (bufSource[i] != '\n') {
        gpio_set_value(lcd_pins[RS], 0);

        // special characters
        // '^' is the character for LCD clear
        if (bufSource[i] == '@') {
            write_lcd(0x01);
            cur_pos = 0;
        // '^' is the character for cursor to shift to opposite row
        } else if (bufSource[i] == '^') {
        	// if cursor on first line, shift to bottom line
        	if (cur_pos <= 15) {
                write_lcd((uint8_t) cur_pos + 64 + 128);
                cur_pos = cur_pos + 16;
        	} else if (cur_pos <= 31) {
                write_lcd((uint8_t) cur_pos + 48 - 64 + 128);
                cur_pos = cur_pos - 16;
            }
        // '<' is the character for cursor shift left
        } else if (bufSource[i] == '<') {
        	// Edge case, set DD RAM to last address of second line of screen
        	if (cur_pos == 0) {
        		write_lcd(0xcf);
        		cur_pos = 31;
        	// Edge case, set DD RAM to last address of first line of screen
        	} else if (cur_pos == 16) {
        		write_lcd(0x8f);
        		cur_pos = 15;
        	} else {
        		write_lcd(0x10);
            	cur_pos--;
        	}
        // '>' is the character for cursor shift right
        } else if (bufSource[i] == '>') {
        	// Edge case, set DD RAM to first address of second line of screen
        	if (cur_pos == 15) {
        		write_lcd(0xc0);
        		cur_pos = 16;
        	// Edge case, set return home
        	} else if (cur_pos == 31) {
        		write_lcd(0x02);
        		cur_pos = 0;
        	} else {
        		write_lcd(0x14);
            	cur_pos++;
        	}
        // normal character entry
        } else {
	        gpio_set_value(lcd_pins[RS], 1);
	        write_lcd((uint8_t) bufSource[i]);
	        cur_pos++;
        }
        i++;

        // if first line is full, set curser to line 2
        gpio_set_value(lcd_pins[RS], 0);
        if (cur_pos == 16) {
            write_lcd(0xc0);
        // if LCD is full, set cursor home
        } else if (cur_pos == 32) {
            write_lcd(0x02);
            cur_pos = 0;
        }
    }
    return rtn;
}

module_init(lcd_start);
module_exit(lcd_exit);
