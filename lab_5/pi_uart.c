/* pi_uart.c
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 2 / June / 2017
 *
 * Initialize UART on Raspberry Pi and send direction signal to BeagleBone
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>

int uart0_filestream = -1;

int initUART() {
    struct termios options;

    // open UART port on Raspberry Pi
    uart0_filestream = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);   //Open in non blocking read/write mode
    
    if (uart0_filestream == -1) {
        perror("open_port: Unable to open /dev/ttyS0...");
        return(-1);
    }

    // UART setting
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);
    return 0;
}

int main() {
    initUART();
    char buf[100];
    int n;
    int k;

    // continuously sends steering commands to BeagleBone
    while (1) {
	    uart0_filestream = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);	

        // Write to the port
        n = write(uart0_filestream,"H",1);

        if (n < 0) {
            perror("Write failed...");
            return -1;
        }

        close(uart0_filestream);
	    usleep(10000);
    }
    return 0;
}
