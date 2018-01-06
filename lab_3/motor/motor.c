/* motor.c
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 3 / May / 2017
 *
 * Motor controller program that reads from the fifo and controls the motors and
 * prints to LCD.
 */

#include "motor.h"

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

// Exports all the PWM pins in pin_nums.
// len_num is the number of pins.
void export_pwm_pins(char** pin_nums, int len_num) {
	FILE *sys;
	int i;
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(sys, 0, SEEK_SET);
	fprintf(sys, "%s", "am33xx_pwm");
	fflush(sys);
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
	char directory[35]; // ask Novin
	sprintf(directory, "/sys/class/gpio/gpio%s/direction", pin_num); //test will null
	dir_file = fopen(directory, "w");
	fprintf(dir_file, "%s", io);
	fflush(dir_file);
	fclose(dir_file);
	return;
}

// Spins the motors forward
void forward(FILE *ain1, FILE *ain2, FILE *bin1, FILE *bin2) {
	fprintf(ain1, "%d", 1);
	fflush(ain1);
	fprintf(ain2, "%d", 0);
	fflush(ain2);

	fprintf(bin1, "%d", 0);
	fflush(bin1);
	fprintf(bin2, "%d", 1);
	fflush(bin2);
}

// Spins motors backward
void backward(FILE *ain1, FILE *ain2, FILE *bin1, FILE *bin2) {
	fprintf(ain1, "%d", 0);
	fflush(ain1);
	fprintf(ain2, "%d", 1);
	fflush(ain2);

	fprintf(bin1, "%d", 1);
	fflush(bin1);
	fprintf(bin2, "%d", 0);
	fflush(bin2);
}

// Stops the motors
void stop(FILE *ain1, FILE *ain2, FILE *bin1, FILE *bin2) {
	fprintf(ain1, "%d", 0);
	fflush(ain1);
	fprintf(ain2, "%d", 0);
	fflush(ain2);

	fprintf(bin1, "%d", 0);
	fflush(bin1);
	fprintf(bin2, "%d", 0);
	fflush(bin2);
}

// Spins one motor to turn left
void left(FILE *ain1, FILE *ain2, FILE *bin1, FILE *bin2) {
	fprintf(ain1, "%d", 0);
	fflush(ain1);
	fprintf(ain2, "%d", 0);
	fflush(ain2);

	fprintf(bin1, "%d", 0);
	fflush(bin1);
	fprintf(bin2, "%d", 1);
	fflush(bin2);
}

// Spins one motor to turn right
void right(FILE *ain1, FILE *ain2, FILE *bin1, FILE *bin2) {
	fprintf(ain1, "%d", 1);
	fflush(ain1);
	fprintf(ain2, "%d", 0);
	fflush(ain2);

	fprintf(bin1, "%d", 0);
	fflush(bin1);
	fprintf(bin2, "%d", 0);
	fflush(bin2);
}

int main() {
	system("insmod /root/lab_3/lcd_sr.ko");
	system("mknod /dev/lcd_sr_device c 239 0");
	// Export GPIO pins
	char* gpio_pins[GPIO_PIN_COUNT] = {AIN1, AIN2, BIN2, BIN1};
	export_gpio_pins(gpio_pins, GPIO_PIN_COUNT);

	// Export PWM pins
	char* pwm_pins[PWM_PIN_COUNT] = {PWMA, PWMB};
	export_pwm_pins(pwm_pins, PWM_PIN_COUNT);

	// Change the direction of GPIO pins
	uint8_t i;
	for (i = 0; i < GPIO_PIN_COUNT; i++) {
		change_dir(gpio_pins[i], "out");
	}

	// Open the GPIO value
	FILE *ain1, *ain2, *bin2, *bin1;
	ain1 = fopen("/sys/class/gpio/gpio67/value", "w");
	ain2 = fopen("/sys/class/gpio/gpio68/value", "w");
	bin2 = fopen("/sys/class/gpio/gpio44/value", "w");
	bin1 = fopen("/sys/class/gpio/gpio26/value", "w");

    /* ***FOR FUTURE USE***
	// Open period, duty cycle, and run for the PWMA
	//FILE *period_a = fopen("/sys/devices/ocp.3/pwm_test_P8_13.15/period", "w");
	//FILE *duty_a = fopen("/sys/devices/ocp.3/pwm_test_P8_13.15/duty", "w");
	//FILE *run_a = fopen("/sys/devices/ocp.3/pwm_test_P8_13.15/run", "w");

	// Open period, duty cycle, and run for the PWMB
	//FILE *period_b = fopen("/sys/devices/ocp.3/pwm_test_P8_19.16/period", "w");
	//FILE *duty_b = fopen("/sys/devices/ocp.3/pwm_test_P8_19.16/duty", "w");
	//FILE *run_b = fopen("/sys/devices/ocp.3/pwm_test_P8_19.16/run", "w");
	*/

	// Continiously read from the fifo
	int lcd = open("/dev/lcd_sr_device", O_RDWR);
	assert(mkfifo(myfifo, 0666)==0);
	while(1) {
			int fd;
			char buf[MAX_BUF];
			while((fd = open( myfifo, O_RDONLY)) == -1);
			read(fd, buf, MAX_BUF);
			close(fd);
			fprintf(stdout,"%s\n",buf);
			write(lcd,"@\n",2);
			// If f character move forward
			if(buf[0] == 'f') {
					forward(ain1, ain2, bin1, bin2);
					write(lcd, "Forward\n", 8);
			// If s then stop
			} else if(buf[0] == 's') {
					stop(ain1, ain2, bin1, bin2);
					write(lcd, "Stop\n", 5);
			// If l turn left
			} else if(buf[0] == 'l') {
					left(ain1, ain2, bin1, bin2);
					write(lcd, "Left\n", 5);
			// If r turn right
			} else if(buf[0] == 'r') {
					right(ain1, ain2, bin1, bin2);
					write(lcd, "Right\n", 6);
			// If b move backwards
			} else if(buf[0] == 'b') {
					backward(ain1, ain2, bin1, bin2);
					write(lcd, "Backward\n", 9);
			}
	}
	fclose(ain1);
	fclose(ain2);
	fclose(bin1);
	fclose(bin2);
	unlink(myfifo);

	return 0;
}
