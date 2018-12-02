#include <inits_tiva.h>
#include "lcd.h"
#include "timer.h"
#include "uart.h"
#include "math.h"
#include "move.h"

#define tpd 153
#define zeroPos 7500
#define power -1.086
#define coe 65430


//data array from sweep
volatile float sweepData[2][91];


//array of objects found during sweep
volatile struct block finds[6];
//number of objects found during sweep
volatile int objectCount = 0;

direction dir;
state s;
int x,y;

int main(void)
{
	int i;
	for(i = 0; i < 6; i++)
	{
		finds[i].width = 0;
		finds[i].center = 0;
		finds[i].distance = 0;
		finds[i].x = 0;
		finds[i].y = 0;
	}
	dir = Forward;
	s = forward;
	oi_t *sensor = oi_alloc();
	oi_init(sensor);

	ping_init();
	adc_init();
	PWM_init();
	lcd_init();

	while(1)
	{
		objectCount = 0;
		move_servo(0);
		timer_waitMillis(300);
		read_turret(&sweepData, &finds, &objectCount);
		char str[100];
		char temp[20];
		sprintf(str, "9,%d", objectCount);
		int LorS;
		int i;
		for(i = 0; i < objectCount; i++)
		{
			if(finds[i].width < 10)
			{
				LorS = 0;
			}
			else
			{
				LorS = 1;
			}
			sprintf(temp, ",%d,%d,%d", LorS, (int) finds[i].x, (int) finds[i].y);
			strcat(str, temp);
		}
		strcat(str, ",e");
		lcd_printf(str);


	}
}
