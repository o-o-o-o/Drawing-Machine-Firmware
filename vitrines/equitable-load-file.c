#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#define LEFT 4
#define RIGHT 5

/* To compile:
	gcc -Wall -o wiring-pi-1 wiring-pi-1.c -lwiringPi
*/

/* Forward Declarations */
void quickflash(void);

int fileindex = 0;

int main(void)
{
	char *files[] = {
		"vitrine1.txt",
		"vitrine2.txt",
		"vitrine3.txt",
		"vitrine4.txt"
	};

	int delaytime = 500;

	wiringPiSetup();
	pinMode(1, OUTPUT);
	pinMode(4, INPUT);
	pinMode(5, INPUT);

	for(;;)
	{
		if(digitalRead(LEFT) == 0){
			quickflash();
			fileindex = (fileindex + 1) % 4;
		}

		if(digitalRead(RIGHT) == 0) {
			quickflash();
			printf("/home/pi/vitrines/%s\n", files[fileindex]);
			return 0;
		}

		digitalWrite(1, HIGH); delay(delaytime);
		digitalWrite(1, LOW); delay(delaytime);
	}
	return 0;
}

void quickflash(void){
	int j = 0;

	for(j = 0; j < 20; j++){
		digitalWrite(1, HIGH); delay(25);
		digitalWrite(1, LOW); delay(25);
	}
}
