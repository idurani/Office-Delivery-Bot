/**
 * @file sensors.c
 * @brief this file contains the functions for the sensors
 * @author Jared Rickard
 * @date 12/05/2018
 */
#include <sensors.h>

///ticks per degree (for move_servo())
#define tpd 153
///match value for zero position of servo (for move_servo())
#define zeroPos 7500
///power constant for ir distance calculation (read_ir())
#define power -1.086
///coefficient constant for ir distance calculation (read_ir())
#define coe 65430

///value of adc for ir sensor
int adc = 1;
///distance from ir sensor
float cm = 0;
///data array from sweep
volatile float sweepData[2][91];
///array of objects found during sweep
volatile struct block finds[6];
///number of objects found during sweep
volatile int objectCount = 0;
///string to be sent over UART to the GUI
char str[50];

/**
 * send a sound pulse from the ping sensor
 * @author Jared Rickard
 * @date 12/05/2018
 */
void sendPulse(void)
{
    //disable alternate function
    GPIO_PORTB_AFSEL_R &= ~0b00001000;
    //set as output
    GPIO_PORTB_DIR_R |= 0b00001000;
    //set pin high
    GPIO_PORTB_DATA_R |= 0b00001000;
    //wait 5 us
    timer_waitMicros(5);
    //set pin low
    GPIO_PORTB_DATA_R &= 0b11110111;
    //set as input
    GPIO_PORTB_DIR_R &= 0b11110111;
    //re-enable alternate function
    GPIO_PORTB_AFSEL_R |= 0b00001000;
}

/**
 * compute the distance for a sound pulse from the ping sensor
 * @return the distance measured by the ping sensor
 * @author Jared Rickard
 * @date 12/05/2018
 */
float read_ping(){
    ///beginning time of the pulse
    unsigned int startTime = 0;
    ///ending time of the pulse
    unsigned int endTime = 0;
    ///total time elapsed for the pulse to start and return
    signed int delta = 0;
    ///send a pulse from the ping sensor
    sendPulse();
    ///wait 115 us as defined in the sensor's manual
    timer_waitMicros(115);
    //reset the interrupt flags
    TIMER3_ICR_R = 0xFFFFFFFF;
    ///wait for an event from the timer
    while ((TIMER3_RIS_R & TIMER_RIS_CBERIS) == 0)
    {}
    ///record the start time of delta
    startTime = TIMER3_TBPS_R & 0x000000FF;
    startTime = startTime << 16;
    startTime += (TIMER3_TBR_R & 0x0000FFFF);
    ///clear the interrupt indicating start of delta
    TIMER3_ICR_R |= 0x00000400;
    ///wait for an event from the timer
    while ((TIMER3_RIS_R & TIMER_RIS_CBERIS) == 0)
    {}
    ///record the end time of delta
    endTime = TIMER3_TBPS_R & 0x000000FF;
    endTime = endTime << 16;
    endTime += (TIMER3_TBR_R & 0x0000FFFF);
    ///clear the interrupt indicating end of delta and overflow of the counter
    TIMER3_ICR_R |= 0x00000400;
    ///change the delta calculation depending on if there is overflow
    delta = startTime - endTime;
    if(delta < 0)
    {
        delta = startTime + (0x0000FFFF - endTime);
    }
    ///convert register values to distance and return the distance
    return 0.0010625 * delta;
}


/**
 * update the ADC register with the new value of IR sensor
 * @author Jared Rickard
 * @date 12/05/2018
 */
void read_ir(){
	///initialize sampling
    ADC0_PSSI_R=ADC_PSSI_SS0;
    ///wait for a sample to finish
    while((ADC0_RIS_R & ADC_RIS_INR0) == 0){
        }
    ///take sample data into main function
    adc = ADC0_SSFIFO0_R & 0x00000FFF;
    ///clear the interrupt
    ADC0_ISC_R = 0x0000001;
    ///wait 0.02s
    timer_waitMillis(50);
    ///find the distance with the calibrated constant from above
    cm = coe * pow((float) adc, power);
}

/**
 * get the raw ADC value from the last time the IR sensor was used
 * @return the register value from the last time the IR sensor was activated
 * @author Jared Rickard
 * @date 12/05/2018
 */
int get_ir_raw(){
    return adc;
}

/**
 * get the distance measured by the IR sensor the last time it was used
 * @return the distance measured the last time the IR sensor was activated
 * @author Jared Rickard
 * @date 12/05/2018
 */
float get_dis_ir(){
    return cm;
}

/**
 * move the turret servo by a specified amount
 * @param deg the amount of degrees to turn the servo
 * @author Jared Rickard
 * @date 12/05/2018
 */
void move_servo(float deg)
{
    ///calculate the position
    float posChangeFloat = 320000 - (zeroPos + (tpd * deg));
    //cast to int so it can be written to registers
    int posChange = (int) posChangeFloat;
    ///write to match registers
    TIMER1_TBMATCHR_R = posChange;
    TIMER1_TBPMR_R = posChange >> 16;
}

/**
 * sweep the sensor turret 180 degrees, recording data from the
 * ping and IR sensors every 2 degrees
 * @author Jared Rickard
 * @date 12/05/2018
 */
void sweep_servo(){
    ///sweep the servo across 180 degrees, 2 degrees at a time
    int i;
    for(i = 0; i < 91; i += 1){
        ///move by two degrees
        move_servo(i*2);
        ///give the servo time to move
        timer_waitMillis(30);
        ///read IR data
        read_ir();
        ///save data into the next section of the array
        sweepData[0][i] = get_dis_ir();
        sweepData[1][i] = read_ping();
        //sprintf(str, "%d\t%.1f\t\t%.1f\n\r", i*2, sweepDataPtr[0][i], sweepDataPtr[1][i] );
        //uart_sendStr(str);
    }
}

/**
 *	sweep the sensor and parse through the data, finding the
 *	first six objects that are larger than 2 degrees
 *	@author Jared Rickard
 *	@date 12/05/2018
 */
void read_turret(){
    ///record data
    sweep_servo();
    ///reset the previous objectCount
    objectCount = 0;
    ///variable to iterate through a for loop
    int i;
    ///boolean value to determine if an object is currently being looked at
    int looking = 0;
    ///the width of the object being looked at
    int currentWidth = 0;
    ///the closest that the object being looked at is to the turret sensors
    float minDist = 100;
    ///start iterating through data gathered
    for(i = 0; i < 90; i++)
    {
        ///if it finds an object, set the boolean variable indicating an object found to 1
        if(sweepData[0][i] < 70 && sweepData[1][i] < 50 && looking == 0
                && sweepData[0][i+1] < 100 && sweepData[1][i+1])
        {
            looking = 1;
        }
        ///if the end of an object is found
        if((sweepData[0][i] >= 70 || sweepData[1][i] >= 50) && looking == 1)
        {
            ///set boolean variable indicating an object found to 0
            looking = 0;
            ///record distance
            finds[objectCount].distance = minDist;
            ///compute center of object
            finds[objectCount].center = i*2-currentWidth;
            ///compute width of object using arc length formula
            finds[objectCount].width = currentWidth * (6.28 / 180) * minDist;
            ///compute x coordinate of object
            finds[objectCount].x = (finds[objectCount].distance + (finds[objectCount].width / 2)) * cos(finds[objectCount].center * 3.14 / 180);
            ///compute y coordinate of object
            finds[objectCount].y = (finds[objectCount].distance + (finds[objectCount].width / 2)) * sin(finds[objectCount].center * 3.14 / 180);
            ///iterate to the next block in the block array
            objectCount = objectCount + 1;
            ///reset helper variables
            currentWidth = 0;
            minDist = 100;
        }
        ///if currently looking at a block
        if(looking == 1)
        {
            ///increase the width helper variable
            currentWidth++;
            ///make sure the minimum distance of the object is still the minimum distance
            if(sweepData[1][i] < minDist)
            {
                minDist = sweepData[1][i];
            }
        }
        ///if the block array is full
        if(objectCount == 6)
        {
            break;
        }
    }
}

/**
 * This method scans the area in front of the robot for objects
 * and sends this information to the GUI
 * @author Jared Rickard
 * @date 12/05/2018
 */
void scan()
{
    ///start off with zero objects
    objectCount = 0;
    ///read the data and finds object width, angle, and distance from the robot
    read_turret();
    ///temporary string to help build the string sent to the robot
    char temp[20];
    sprintf(str, "9,%d", objectCount);
    ///determine if the object is large or small
    int LorS;
    //iterate through all the objects found during the scan
    int i;
    for(i = 0; i < objectCount; i++)
    {
    	///if the object's width was smaller than 7 cm
        if(finds[i].width < 7)
        {
        	///the object was a small object
            LorS = 0;
        }
        ///else the object's width was larger than 7 cm
        else
        {
        	///the object was a large object
            LorS = 1;
        }
        ///add the relevant to the string being sent to the GUI
        sprintf(temp, ",%d,%d,%d", LorS, (int) finds[i].x, (int) finds[i].y);
        strcat(str, temp);
    }
    ///add the final delimiter
    strcat(str, ",e");
    ///send object data to the GUI
    uart_sendStr(str);
    ///reset the position of the sensor turret
    move_servo(0);
}
