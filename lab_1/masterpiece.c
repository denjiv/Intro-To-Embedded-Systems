/* masterpiece.c
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 5 / Apr / 2017
 *
 * Blinks LEDs in sequence while playing sounds from a buzzer
 */

#include <stdio.h>      // for File IO and printf
#include <time.h>       // for usleep
#include <stdint.h>
//#include <assert.h>

#define GPIO_PIN_COUNT 3
#define GPIO_PIN_A "44"
#define GPIO_PIN_B "45"
#define GPIO_PIN_C "46"

#define PWM_PIN_COUNT 1
#define PWM_PIN_A "bone_pwm_P8_13"

#define MASK_1 0x1
#define MASK_2 0x2
#define MASK_3 0x4

#define PWM_PIN "bone_pwm_P8_13"

// note frequencies for super mario
#define NOTE_E7 2637
#define NOTE_C7 2093
#define NOTE_G7 3136
#define NOTE_G6 1568
#define NOTE_PAUSE 1

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

int main() {
	// Export GPIO pins 
	char* gpio_pins[GPIO_PIN_COUNT] = {GPIO_PIN_A, GPIO_PIN_B, GPIO_PIN_C};
	export_gpio_pins(gpio_pins, GPIO_PIN_COUNT);
	
	// Export PWM pins
	char* pwm_pins[PWM_PIN_COUNT] = {PWM_PIN_A};
	export_pwm_pins(pwm_pins, PWM_PIN_COUNT);

	// Change the direction of GPIO pins
	uint8_t i;
	for (i = 0; i < GPIO_PIN_COUNT; i++) {
		change_dir(gpio_pins[i], "out");
	}
	
	// Open the GPIO value 
	FILE *val1, *val2, *val3;
	val1 = fopen("/sys/class/gpio/gpio44/value", "w");
	val2 = fopen("/sys/class/gpio/gpio45/value", "w");
	val3 = fopen("/sys/class/gpio/gpio46/value", "w");

	// Open period, duty cycle, and run for the PWM
	FILE *period = 0;
	// While loop was added to account for Segmentation Fault
	while (period == NULL) {
		period = fopen("/sys/devices/ocp.3/pwm_test_P8_13.15/period", "w");
	}
	// assert(period != NULL);
	FILE *duty = fopen("/sys/devices/ocp.3/pwm_test_P8_13.15/duty", "w");
	// assert(duty != NULL);
	FILE *run = fopen("/sys/devices/ocp.3/pwm_test_P8_13.15/run", "w"); // run = enable
	// assert(run != NULL);
	
	// Define the melody notes and tempo	
	int melody[] = {NOTE_E7, NOTE_E7, NOTE_PAUSE, NOTE_E7, NOTE_C7, NOTE_E7, NOTE_G7, NOTE_G6};
	int tempo[] = {150000, 350000, 100000, 400000, 150000, 400000, 700000, 700000};
	int note_period = 0;
	uint8_t count = 0x0;
	while(1) {
		// Restart counter
		count = (count % 8);
		uint8_t masked_1 = count & MASK_1;
		uint8_t masked_2 = (count & MASK_2) >> 1;
		uint8_t masked_3 = (count & MASK_3) >> 2;
		
		// Assign led values
		fprintf(val1, "%d", masked_1);
		fflush(val1);
		fprintf(val2, "%d", masked_2);
		fflush(val2);
		fprintf(val3, "%d", masked_3);
		fflush(val3);
		
		// Play melody
		if (melody[count] == NOTE_PAUSE) {
			fprintf(duty, "%d", 0);
			fflush(duty);
		} else {
			note_period = 1.0*1000000000/melody[count]*4; // 10
			fprintf(period, "%d", note_period);
			fflush(period);
			fprintf(duty, "%d", note_period/2);
			fflush(duty);
		}
		

		// Toggling on and off PWM
		fprintf(run, "%d", 1);
		fflush(run);
		usleep(tempo[count]);
		fprintf(run, "%d", 0);
		fflush(run);
		
		count += 0x1;

	}

	// close all files
	fclose(val1);
	fclose(val2);
	fclose(val3);
	fclose(period);
	fclose(duty);
	fclose(run);
	return 0;
}
