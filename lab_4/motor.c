/* motor.c
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 3 / May / 2017
 *
 * main file for motor control and reading proximity sensors
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

// command tank to go forward
void forward() {
	fprintf(ain1, "%d", 1);
	fflush(ain1);
	fprintf(ain2, "%d", 0);
	fflush(ain2);

	fprintf(bin1, "%d", 0);
	fflush(bin1);
	fprintf(bin2, "%d", 1);
	fflush(bin2);
}

// command tank to go backward
void backward() {
	fprintf(ain1, "%d", 0);
	fflush(ain1);
	fprintf(ain2, "%d", 1);
	fflush(ain2);

	fprintf(bin1, "%d", 1);
	fflush(bin1);
	fprintf(bin2, "%d", 0);
	fflush(bin2);
}

// command tank to stop
void stop() {
	fprintf(ain1, "%d", 0);
	fflush(ain1);
	fprintf(ain2, "%d", 0);
	fflush(ain2);

	fprintf(bin1, "%d", 0);
	fflush(bin1);
	fprintf(bin2, "%d", 0);
	fflush(bin2);
}

// command tank to go left
void left() {
	fprintf(ain1, "%d", 0);
	fflush(ain1);
	fprintf(ain2, "%d", 1);
	fflush(ain2);

	fprintf(bin1, "%d", 0);
	fflush(bin1);
	fprintf(bin2, "%d", 1);
	fflush(bin2);
}

// command tank to go right
void right() {
	fprintf(ain1, "%d", 1);
	fflush(ain1);
	fprintf(ain2, "%d", 0);
	fflush(ain2);

	fprintf(bin1, "%d", 1);
	fflush(bin1);
	fprintf(bin2, "%d", 0);
	fflush(bin2);
}

// helper function for adjusting PWM duty cycle
void set_duty(FILE* duty, int percent) {
	fprintf(duty, "%d", (int) (((int) ((1.0/FREQ) * pow(10.0, 9))) * percent/100.0));
	fflush(duty);
}

// function to initialize the timer
void timer_init() {
    spec.it_value.tv_sec = 0;
    spec.it_value.tv_nsec = 100000000;

    spec.it_interval.tv_sec = 0;
    spec.it_interval.tv_nsec = 100000000;

    timer_create(CLOCK_REALTIME, NULL, &timerID);
    timer_settime(timerID, 0, &spec, NULL);

}

// function to stop the timer
void timer_end() {
    spec.it_value.tv_sec = 0;
    spec.it_value.tv_nsec = 0;
    spec.it_interval.tv_sec = 0;
    spec.it_interval.tv_nsec = 0;

    timer_settime(timerID, 0, &spec, NULL);
}

// helper function for reading ADC values
int read_adc(int pin)
{
	 int fd;
	 char buf[36];
	 char adc_val[4];
	 snprintf(buf, sizeof(buf), "/sys/devices/ocp.3/helper.17/AIN%d", pin);
	 fd = open(buf, O_RDONLY);
	 read(fd, &adc_val, 4);
	 close(fd);
	 return atoi(adc_val);
}

// timer interrupt handler
void timer_handler() {
	if(autoT == 1 && buf[0] == 'a') {
		int front_sensor = read_adc(0);
		fprintf(stdout,"front_sensor: %d\n", front_sensor);
		int rear_sensor = read_adc(2);
		fprintf(stdout,"rear_sensor: %d\n", rear_sensor);
		int left_sensor = read_adc(6);
		fprintf(stdout,"left_sensor: %d\n", left_sensor);
		int right_sensor = read_adc(4);
		fprintf(stdout,"right_sensor: %d\n", right_sensor);

		// logics here
		if (front_sensor < THRESHOLD) {
			forward();
		} else if (left_sensor < THRESHOLD) {
			left();
		} else if (right_sensor < THRESHOLD) {
			right();
		} else if (rear_sensor < THRESHOLD) {
			backward();
		} else {
			stop();
		}
	}
}

int main() {
	//system("insmod /root/lab_3/lcd_sr.ko");
	//system("mknod /dev/lcd_sr_device c 239 0");
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
	ain1 = fopen("/sys/class/gpio/gpio67/value", "w");
	ain2 = fopen("/sys/class/gpio/gpio68/value", "w");
	bin2 = fopen("/sys/class/gpio/gpio44/value", "w");
	bin1 = fopen("/sys/class/gpio/gpio26/value", "w");

	// Open period, duty cycle, and run for the PWMA
	FILE *period_a = fopen("/sys/devices/ocp.3/pwm_test_P8_13.15/period", "w");
	FILE *duty_a = fopen("/sys/devices/ocp.3/pwm_test_P8_13.15/duty", "w");
	FILE *run_a = fopen("/sys/devices/ocp.3/pwm_test_P8_13.15/run", "w");

	// Open period, duty cycle, and run for the PWMB
	FILE *period_b = fopen("/sys/devices/ocp.3/pwm_test_P8_19.16/period", "w");
	FILE *duty_b = fopen("/sys/devices/ocp.3/pwm_test_P8_19.16/duty", "w");
	FILE *run_b = fopen("/sys/devices/ocp.3/pwm_test_P8_19.16/run", "w");

	// setup pwm
	fprintf(period_a, "%d", ((int) ((1.0/FREQ) * pow(10.0, 9))));
	fflush(period_a);
	fprintf(period_b, "%d", ((int) ((1.0/FREQ) * pow(10.0, 9))));
	fflush(period_b);

	fprintf(run_a, "%d", 1);
	fflush(run_a);
	fprintf(run_b, "%d", 1);
	fflush(run_b);

	set_duty(duty_a, 15);
	set_duty(duty_b, 15);
	// Continiously read from the fifo
	assert(mkfifo(myfifo, 0666) == 0);

	// setup adc pins on the board
	system("echo BB-ADC > /sys/devices/bone_capemgr.9/slots");

	// timer interrupt setup
	signal(SIGALRM, timer_handler);
	autoT = 0;
	timer_init();
    while(1) {
		int fd;
		while((fd = open(myfifo, O_RDONLY)) == -1);
		read(fd, buf, MAX_BUF);
		close(fd);
		fprintf(stdout,"%s\n",buf);
		fprintf(stdout,"autoT: %d\n", autoT);
		if(buf[0] == 'a') {
				if(autoT == 0) {
					stop();
					autoT = 1;
				}
		} else if(buf[0] == 'f') {
				autoT == 0;
				forward();
		} else if(buf[0] == 's') {
				autoT == 0;
				stop();
		} else if(buf[0] == 'l') {
				autoT == 0;
				left();
		} else if(buf[0] == 'r') {
				autoT == 0;
				right();
		} else if(buf[0] == 'b') {
				autoT == 0;
				backward();
		}
	}
    timer_end();
	fclose(ain1);
	fclose(ain2);
	fclose(bin1);
	fclose(bin2);
	unlink(myfifo);
	fclose(period_a);
	fclose(duty_a);
	fclose(run_a);
	fclose(period_b);
	fclose(duty_b);
	fclose(run_b);

	return 0;
}
