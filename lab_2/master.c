/* master.c
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 10 / Apr / 2017
 *
 * Repeatly used methods across lab 2 scripts
 */

// Returns the size of the string
int size_str(char* str) {
	int size = 0;
	while (str[size] != '\0') {
		size++;
	}
	return size;
}

// Writes to the fifo pipe
void write_fifo(char* str) {
	int fd;
	fd = open(myfifo, O_WRONLY);
	assert(fd != -1);
	int size_string = size_str(str);
	write(fd, str, size_string);
	close(fd);
	return;
}

// Flushed the stdin; removes the oveflow user input (> 32 chars)
void flush_stdin() {
	int ch;
	while(((ch = getchar()) != '\n') && (ch != EOF));
}