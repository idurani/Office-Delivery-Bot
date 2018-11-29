#include <inits_tiva.h>
#include "lcd.h"
#include "timer.h"
#include "uart.h"
#include "math.h"

#define tpd 153
#define zeroPos 7500
#define power -1.086
#define coe 65430



volatile float sweepData[2][91];
struct block
{
	float width;
	int center;
	float distance;

};





void sweep_servo(){
	//char str[50];
	//sprintf(str, "Deg\tIR Distance(cm)\tSonarDistance(cm)\n\r");
	int i;
	//sweep the servo across 180 degrees, 2 degrees at a time
	for(i = 0; i < 91; i += 1){
		//move by two degrees
		move_servo(i*2);
		//give the servo time to move
		timer_waitMillis(20);
		//read ir data
		read_ir();
		//save data into the next section of the array
		sweepData[0][i] = get_dis_ir();
		sweepData[1][i] = read_ping();
		//sprintf(str, "%d\t%.1f\t\t%.1f\n\r", i*2, sweepData[0][i], sweepData[1][i] );
		//uart_sendStr(str);
	}
}

volatile struct block finds[6];
volatile int objectCount = 0;

void read_turret(){
	//record data
		sweep_servo();
	//variables for iterating through data to find blocks and info about blocks
		objectCount = 0;
		int i;
		int looking = 0;
		int currentWidth = 0;
		float minDist = 100;

		//start iterating through data gathered
		for(i = 0; i < 90; i++)
		{
			//if it finds an object, set the boolean variable indicating an object found to 1
			if(sweepData[0][i] < 100 && sweepData[1][i] < 50 && looking == 0)
			{
				looking = 1;
			}
			//if the end of an object is found
			if((sweepData[0][i] >= 100 || sweepData[1][i] >= 50) && looking == 1)
			{
				//set boolean variable indicating an object found to 0
				looking = 0;
				//record distance
				finds[objectCount].distance = minDist;
				//compute center of object
				finds[objectCount].center = i*2-currentWidth;
				//compute width of object using arc length formula
				finds[objectCount].width = currentWidth * (6.28 / 180) * minDist;
				//iterate to the next block in the block array
				objectCount++;
				//reset helper variables
				currentWidth = 0;
				minDist = 100;
			}
			//if currently looking at a block
			if(looking == 1)
			{
				//increase the width helper variable
				currentWidth++;
				//make sure the minimum distance of the object is still the minimum distance
				if(sweepData[1][i] < minDist)
				{
					minDist = sweepData[1][i];
				}
			}
			//if the block array is full
			if(objectCount == 6)
			{
				break;
			}
		}
}

//boolean values to determine state machine
volatile int commandGiven;
volatile int objectDetected;
volatile int wallDetected;
volatile int bumpDetected;
volatile int xAligned;
volatile int yAligned;
volatile int fiftyCMReached;
volatile int turnedForWall;
volatile int desLocated;

//integer values for calculations
volatile int xCoord;
volatile int yCoord;
volatile int distanceTraversed;
volatile int xDes;
volatile int yDes;

/**
 * state after state machine calculations
 * 1: waiting for user commands
 * 2: scanning the way in front
 * 3: unimpeded movement
 * 4: object in path movement
 * 5: wall in path movement
 * 6: bump detected movement
 * 7: axis aligned movement
 * 8: destination located
 * 9: perform action
 */
volatile int state;

int main(void)
{
	//initialize the boolean and integer values
	commandGiven = 0;
	objectDetected = 0;
	wallDetected = 0;
	bumpDetected = 0;
	xAligned = 0;
	yAligned = 0;
	fiftyCMReached = 0;
	turnedForWall = 0;
	desLocated = 0;
	xCoord = 0;
	yCoord = 0;
	distanceTraversed = 0;
	xDes = -1;
	yDes = -1;
	//initialize the sensors
	ping_init();
	adc_init();
	PWM_init();
	//start infinite while loop
	//setLocation();

	//put in state one
	//userSelect(); // variable named int destination 1, 2, 3 different corner
	while(1)
	{
		//state machine calculations
		//TODO
		//if statements for each state
		//TODO
	}
}
