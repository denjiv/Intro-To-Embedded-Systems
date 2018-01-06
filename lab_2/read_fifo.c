/* read_fifo.c
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 5 / Apr / 2017
 *
 * LCD driver (with read_fifo)
 */

#include "master.h"

// Toggle the enable pin
void write_en() {
	fprintf(lcd_pins[EN], "%d", 0);
	fflush(lcd_pins[EN]);
	usleep(1);
	fprintf(lcd_pins[EN], "%d", 1);
	fflush(lcd_pins[EN]);
	usleep(1);
	fprintf(lcd_pins[EN], "%d", 0);
	fflush(lcd_pins[EN]);
	usleep(1);
}

// Write to the lcd display based on pin values in val
void write_lcd(uint16_t val) {
	uint8_t i;
	usleep(1000);

	for(i = 0; i < GPIO_PIN_COUNT - 1; i++) {
		fprintf(lcd_pins[i], "%d", val & MASK_WRITE);
		fflush(lcd_pins[i]);
		val = val >> 1;
	}

	write_en();
}

// Initialize the display
void init() {
	usleep(20000);
	write_lcd(0x030);
	usleep(5000);
	write_lcd(0x030);
	usleep(500);
	write_lcd(0x030);
	usleep(1000);

	write_lcd(0x038); // function set
	write_lcd(0x008); // Display OFF
	write_lcd(0x001); // Clear Display
	usleep(1000);
	write_lcd(0x006); // Entry Mode
	write_lcd(0x00F); // Display ON
}

// Exports all the GPIO pins in pin_nums.
// len_num is the number of pins.
void export_gpio_pins(char** pin_nums, int len_num) {
	FILE *sys;
	int i;
	sys = fopen("/sys/class/gpio/export", "w");
	fseek(sys, 0, SEEK_SET);

	for(i=0; i < len_num; i++) {
		fprintf(sys, "%s", pin_nums[i]);
		fflush(sys);
	}

	fclose(sys);
	return;
}

// Changes the direction of pin_num to io.
void change_dir(char* pin_num, char* io) {
	FILE *dir_file;
	char directory[35];
	sprintf(directory, "/sys/class/gpio/gpio%s/direction", pin_num); //test will null
	dir_file = fopen(directory, "w");
	fprintf(dir_file, "%s", io);
	fflush(dir_file);
	fclose(dir_file);
	return;
}

// Read from the fifo piep and print to the screen
void read_fifo() {
	int fd;
	char buf[MAX_BUF];
	while((fd = open(myfifo, O_RDONLY)) == -1);
	write_lcd(0x001); // clears display
	read(fd, buf, MAX_BUF);
	close(fd);
	int i = 0;
	while (buf[i] != '\n') {
		if(i == 16) { 			//if first line is full, set curser to line 2
			write_lcd(0x0a8);
		}
		write_lcd((uint16_t) buf[i] | MASK_LCD_WRITE);
		i++;
	}
	return;
}

int main() {
	// Export GPIO pins
	char* gpio_pins[GPIO_PIN_COUNT] = {DB0_PIN, DB1_PIN, DB2_PIN, DB3_PIN, DB4_PIN, DB5_PIN, DB6_PIN, DB7_PIN, RS_PIN, RW_PIN, EN_PIN};
	export_gpio_pins(gpio_pins, GPIO_PIN_COUNT);

	// Change the direction of GPIO pins
	uint8_t i;
	for (i = 0; i < GPIO_PIN_COUNT; i++) {
		change_dir(gpio_pins[i], "out");
	}

	// Open the pin files
	lcd_pins[0] = fopen("/sys/class/gpio/gpio67/value", "w");
	assert(lcd_pins[0] != NULL);
	lcd_pins[1] = fopen("/sys/class/gpio/gpio68/value", "w");
	assert(lcd_pins[1] != NULL);
	lcd_pins[2] = fopen("/sys/class/gpio/gpio44/value", "w");
	assert(lcd_pins[2] != NULL);
	lcd_pins[3] = fopen("/sys/class/gpio/gpio26/value", "w");
	assert(lcd_pins[3] != NULL);
	lcd_pins[4] = fopen("/sys/class/gpio/gpio46/value", "w");
	assert(lcd_pins[4] != NULL);
	lcd_pins[5] = fopen("/sys/class/gpio/gpio65/value", "w");
	assert(lcd_pins[5] != NULL);
	lcd_pins[6] = fopen("/sys/class/gpio/gpio61/value", "w");
	assert(lcd_pins[6] != NULL);
	lcd_pins[7] = fopen("/sys/class/gpio/gpio66/value", "w");
	assert(lcd_pins[7] != NULL);
	lcd_pins[8] = fopen("/sys/class/gpio/gpio69/value", "w");
	assert(lcd_pins[8] != NULL);
	lcd_pins[9] = fopen("/sys/class/gpio/gpio45/value", "w");
	assert(lcd_pins[9] != NULL);
	lcd_pins[10] = fopen("/sys/class/gpio/gpio47/value", "w");
	assert(lcd_pins[10] != NULL);

	init();
	assert(mkfifo(myfifo, 0666)==0);
	// Continue reading from the pipe
	while(1) {
		read_fifo();
	}
	fcloseall();
	unlink(myfifo);
	return 0;
}
