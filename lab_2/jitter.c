/* jitter.c
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 17 / Apr / 2017
 *
 * Demo on scheduler jitter by toggling a GPIO pin from low to high then low
 */

#include <stdio.h>      // for File IO and printf
#include <unistd.h>     // for usleep
#include <stdint.h>

#define LSB_MASK 0x01

int main() {
	FILE* sys = fopen("/sys/class/gpio/export", "w");
	fseek(sys, 0, SEEK_SET);
	fprintf(sys, "%s", "60");
	fflush(sys);
	fclose(sys);

	FILE* gpio_dir = fopen("/sys/class/gpio/gpio60/direction", "w");
	fprintf(gpio_dir, "%s", "out");
	fflush(gpio_dir);
	fclose(gpio_dir);

	uint8_t val = 0x0;
	FILE* gpio = fopen("/sys/class/gpio/gpio60/value", "w");

	while(1) {
		fprintf(gpio, "%d", val & LSB_MASK);
		fflush(gpio);
		val = ~val;
		usleep(1);
	}

	fclose(gpio);
	return 0;
}
