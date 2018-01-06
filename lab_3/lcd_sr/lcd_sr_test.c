/* lcd_sr_test.c
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 3 / May / 2017
 *
 * Userspace test code for the kernel module lcd_sr.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#define NEW_CHAR_DIR "/dev/lcd_sr_device"

// Flushed the stdin; removes the oveflow user input (> 32 chars)
void flush_stdin() {
	int ch;
	while(((ch = getchar()) != '\n') && (ch != EOF));
}

int main(void)
{
	// Open the device
	int fd;
	fd = open(NEW_CHAR_DIR, O_RDWR);
	if (fd < 0)
	{
		printf("File %s cannot be opened\n", NEW_CHAR_DIR);
		exit(1);
	}

	while (1)
	{
		char read_buf[34], write_buf[34];
		// User Input
		fprintf(stdout, "Enter Text to Display(32 character limit):\n");
		fflush(stdout);
		fgets(write_buf, 34, stdin);
		fflush(stdin);

		// User input overflow handeler
		while (strstr(write_buf, "\n\0") == NULL) {
			flush_stdin();
			fprintf(stdout, "You exceeded the limit. Try again(32 character limit):\n");
			fflush(stdout);
			fgets(write_buf, 34, stdin);
			flush_stdin();
		}

		if (strstr(write_buf, "exit()")) {
			break;
		}
		// Update the terminal
		write(fd, write_buf, sizeof(write_buf));
		printf("Successfully wrote to device file\n");
		printf("Enter Text: ");

		read(fd, read_buf, sizeof(read_buf));
		printf("Successfully read from device file\n");
		printf("The words are: %s\n", read_buf);
	}

	close(fd);
	return 0;
}
