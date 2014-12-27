#include <wiringPi.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Takes one argument and sends that argument to PIC via UART */


/* To compile, remember to link with wiring pi library. E.g. :
    gcc -Wall -o equitable-send-command equitable-send-command.c -lwiringPi 
*/


/* Forward Declarations */
int serialOpen (char *device, int baud);
void serialFlush (int fd);
void serialClose (int fd);
void serialPutchar (int fd, unsigned char c);
void serialPuts (int fd, char *s);
void serialPrintf (int fd, char *message, ...);
int serialDataAvail (int fd);
int serialGetchar (int fd);
/*******************************/

int main(int argc, char * argv[]){
	int j;	/* For use in the for loop below. */
	int rx = 1;
	int port;
	char * device = "/dev/ttyAMA0";
	int baudrate = 9600;

	/* Enforce proper Usage */
	if(argc != 2){
		fprintf(stderr,"Usage: ./name-of-program string_to_transmit\n");
		return 1;
	}

	port = serialOpen(device, baudrate);

	/* Error handling: */
	if(port == -1){
		perror("failed to open serial port");
	}

	serialPuts(port, argv[1]);

	rx = serialGetchar(port);
	if(rx == 68){
		return 0;
	} else{
		return 1;
	}
}


/**************************************
*Source for wirinSerial
**************************************/
/*
 * serialOpen:
 *	Open and initialise the serial port, setting all the right
 *	port parameters - or as many as are required - hopefully!
 *********************************************************************************
 */

int serialOpen (char *device, int baud)
{
  struct termios options ;
  speed_t myBaud ;
  int     status, fd ;

  switch (baud)
  {
    case     50:	myBaud =     B50 ; break ;
    case     75:	myBaud =     B75 ; break ;
    case    110:	myBaud =    B110 ; break ;
    case    134:	myBaud =    B134 ; break ;
    case    150:	myBaud =    B150 ; break ;
    case    200:	myBaud =    B200 ; break ;
    case    300:	myBaud =    B300 ; break ;
    case    600:	myBaud =    B600 ; break ;
    case   1200:	myBaud =   B1200 ; break ;
    case   1800:	myBaud =   B1800 ; break ;
    case   2400:	myBaud =   B2400 ; break ;
    case   9600:	myBaud =   B9600 ; break ;
    case  19200:	myBaud =  B19200 ; break ;
    case  38400:	myBaud =  B38400 ; break ;
    case  57600:	myBaud =  B57600 ; break ;
    case 115200:	myBaud = B115200 ; break ;
    case 230400:	myBaud = B230400 ; break ;

    default:
      return -2 ;
  }

  if ((fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1)
    return -1 ;

  fcntl (fd, F_SETFL, O_RDWR) ;

// Get and modify current options:

  tcgetattr (fd, &options) ;

    cfmakeraw   (&options) ;
    cfsetispeed (&options, myBaud) ;
    cfsetospeed (&options, myBaud) ;

    options.c_cflag |= (CLOCAL | CREAD) ;
    options.c_cflag &= ~PARENB ;
    options.c_cflag &= ~CSTOPB ;
    options.c_cflag &= ~CSIZE ;
    options.c_cflag |= CS8 ;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
    options.c_oflag &= ~OPOST ;

    options.c_cc [VMIN]  =   0 ;
    options.c_cc [VTIME] = 200 ;	// Ten seconds (100 deciseconds)

  tcsetattr (fd, TCSANOW | TCSAFLUSH, &options) ;

  ioctl (fd, TIOCMGET, &status);

  status |= TIOCM_DTR ;
  status |= TIOCM_RTS ;

  ioctl (fd, TIOCMSET, &status);

  usleep (10000) ;	// 10mS

  return fd ;
}


/*
 * serialFlush:
 *	Flush the serial buffers (both tx & rx)
 *********************************************************************************
 */

void serialFlush (int fd)
{
  tcflush (fd, TCIOFLUSH) ;
}


/*
 * serialClose:
 *	Release the serial port
 *********************************************************************************
 */

void serialClose (int fd)
{
  close (fd) ;
}


/*
 * serialPutchar:
 *	Send a single character to the serial port
 *********************************************************************************
 */

void serialPutchar (int fd, unsigned char c)
{
  write (fd, &c, 1) ;
}


/*
 * serialPuts:
 *	Send a string to the serial port
 *********************************************************************************
 */

void serialPuts (int fd, char *s)
{
  write (fd, s, strlen (s)) ;
}

/*
 * serialPrintf:
 *	Printf over Serial
 *********************************************************************************
 */

void serialPrintf (int fd, char *message, ...)
{
  va_list argp ;
  char buffer [1024] ;

  va_start (argp, message) ;
    vsnprintf (buffer, 1023, message, argp) ;
  va_end (argp) ;

  serialPuts (fd, buffer) ;
}


/*
 * serialDataAvail:
 *	Return the number of bytes of data avalable to be read in the serial port
 *********************************************************************************
 */

int serialDataAvail (int fd)
{
  int result ;

  if (ioctl (fd, FIONREAD, &result) == -1)
    return -1 ;

  return result ;
}


/*
 * serialGetchar:
 *	Get a single character from the serial device.
 *	Note: Zero is a valid character and this function will time-out after
 *	10 seconds.
 *********************************************************************************
 */

int serialGetchar (int fd)
{
  uint8_t x ;

  if (read (fd, &x, 1) != 1)
    return -1 ;

  return ((int)x) & 0xFF ;
}
