/* write_fifo.c
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 5 / Apr / 2017
 *
 * LCD driver (with write_fifo)
 */

#include "master.h"

int main() {
	while (1) {
		// User Input
		char input[34];
		fprintf(stdout, "Enter text to display(32 character limit):\n");
		fflush(stdout);
		fgets(input, 34, stdin);
		fflush(stdin);

		// User input overflow handeler
		while (strstr(input, "\n\0") == NULL) {
			flush_stdin();
			fprintf(stdout, "You exceeded the limit. Try again(32 character limit):\n");
			fflush(stdout);
			write_fifo("Limit of 32 characters exceeded.\n");
			fgets(input, 34, stdin);
			flush_stdin();
		}
		if (strstr(input, "exit()")) {
			break;
		}
		// Write to the pipe
		write_fifo(input);
	}
	unlink(myfifo);
	return 0;
}
