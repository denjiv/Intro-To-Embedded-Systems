/* uart_drive_pi.c
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 2 / June / 2017
 *
 * Initialize the UART on BeagleBone that is connected to the UART on Pi
 */


#include "motor.h"

int main() {
    char *portname = "/dev/ttyO1";
    int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);

    struct termios tty;
    memset (&tty, 0, sizeof tty);
    tcgetattr (fd, &tty);
    // Set baud rate
    cfsetospeed (&tty, B115200);
    cfsetispeed (&tty, B115200);
    // Enable the reciever and set local mode
    tty.c_cflag |= (CLOCAL | CREAD);
    //Select 8 data bits
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    // Parity bit off
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    // Disable hardware flow control
    tty.c_cflag &= ~CRTSCTS;
    // Do not configure cannonical or raw input
    tty.c_lflag = 0;
    // Disable software flow control
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    // Disable break condition
    tty.c_iflag &= ~IGNBRK;
    // Diable delays
    tty.c_oflag = 0;
    // For simplicity set VMIN to 0 and VTIME to 0.5 seconds of read time
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5;
    tcsetattr (fd, TCSANOW, &tty);

    usleep (10000);
    close(fd);

    // Read data from Pi and pipe into a fifo
    while(1) {
        char buf1;
        int fifo = open("/tmp/pififo", O_WRONLY);
        fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
        read (fd, &buf1, 1);
        fprintf(stdout, "string: %c\n",buf1);
        write(fifo, &buf1, 1);
        close(fifo);
	    close(fd);
    }
    return 0;
}
