/**
 * @file inits_tiva.c
 * @brief this file contains functions for initialization and sensors
 * @author Jared Rickard
 * @date 12/05/2018
 */
#include <inits_tiva.h>
#include "math.h"

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


/**
 * initialize the ping sensor
 * @author Jared Rickard
 * @date 12/05/2018
 */
void ping_init(void)
{
    //enable GPIO clock
    SYSCTL_RCGCGPIO_R |= 0b00000010;
    //enable Digital function of pin 3
    GPIO_PORTB_DEN_R |= 0b00001000;
    //enable timer function of pin 3
    GPIO_PORTB_PCTL_R |= 0x00007000;

    //start Timer clock
    SYSCTL_RCGCTIMER_R |= 0x00000008;
    //disable timer 3b
    TIMER3_CTL_R = 0x00000000;
    //setup timer
    TIMER3_CFG_R = 0x00000004;
    //put timer in capture mode
    TIMER3_TBMR_R |= 0x00000007;
    //set upper limit of timer
    TIMER3_TBILR_R |= 0x0000FFFF;
    //re-enable timer
    TIMER3_CTL_R |= 0x00000D00;
}

/**
 * initialize the IR sensor
 * @author Jared Rickard
 * @date 12/05/2018
 */
void adc_init(void){
    //GPIO initialization
	//enable clock for port b
    SYSCTL_RCGCGPIO_R |= 0b00000010;
    //disable SS0 sample sequencer to configure it
    SYSCTL_RCGCADC_R |= 0b00000001;
    //enable pin 4 to use an alternate function
    GPIO_PORTB_AFSEL_R |= 0b00010000;
    //make sure pin 4 is an input
    GPIO_PORTB_DIR_R &= 0b11101111;
    //disable the digital functionality of pin 4
    GPIO_PORTB_DEN_R &= 0b11101111;
    //enable the analog functionality of pin 4
    GPIO_PORTB_AMSEL_R |= 0b00010000;
    GPIO_PORTB_ADCCTL_R &= 0b11101111;
    //ADC initialization
    //initialize the ADC trigger source as processor (default)
    ADC0_ACTSS_R &= 0b0000000;
    //set 1st sample to use the AIN10 ADC pin
    ADC0_EMUX_R &= 0xFFFFFF0;
    //enable raw interrupt status
    ADC0_SSMUX0_R |= 0x000000A;
    //enable oversampling to average
    ADC0_SSCTL0_R |= 0x00000006;
    ADC0_SAC_R |= ADC_SAC_AVG_64X;
    //re-enable ADC0 SS0
    ADC0_ACTSS_R |= 0x00000001;
}

/**
 * initialize the PWM signal to the turret servo
 * @author Jared Rickard
 * @date 12/05/2018
 */
void PWM_init()
{
    //enable GPIO port B clock
    SYSCTL_RCGCGPIO_R |= 0b00000010;
    //set pin 5 as output
    GPIO_PORTB_DIR_R |= 0b00100000;
    //enable digital functionality of pin 5
    GPIO_PORTB_DEN_R |= 0b00100000;
    //enable alternate function of pin 5
    GPIO_PORTB_AFSEL_R |= 0b00100000;
    //set which alternate function of pin 5
    GPIO_PORTB_PCTL_R |= 0x00700000;

    //enable Timer clock
    SYSCTL_RCGCTIMER_R |= 0b00000010;
    //disable timer 1b
    TIMER1_CTL_R = 0x00000000;
    //set to a 16 bit timer
    TIMER1_CFG_R |= 0x0004;
    //set the timer mode to PWM, periodic timer
    TIMER1_TBMR_R |= 0x000A;

    //load upper limit of 20 ms period
    //20 ms = 320000 ticks which can be loaded into ILR, PR doesn't need anything
    TIMER1_TBILR_R = 0xE200;
    TIMER1_TBPR_R = 0x0004;
    //set initial match to 90 degrees
    TIMER1_TBMATCHR_R = 0x91EC;
    TIMER1_TBPMR_R = 0x0004;
    //re-enable timer
    TIMER1_CTL_R |= 0x0100;
}


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
 * @param sweepDataPtr 	A 2D array that will store the sensor information
 * @author Jared Rickard
 * @date 12/05/2018
 */
void sweep_servo(volatile float (*sweepDataPtr)[2][91]){
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
        (*sweepDataPtr)[0][i] = get_dis_ir();
        (*sweepDataPtr)[1][i] = read_ping();
        //sprintf(str, "%d\t%.1f\t\t%.1f\n\r", i*2, sweepDataPtr[0][i], sweepDataPtr[1][i] );
        //uart_sendStr(str);
    }
}

/**
 *	sweep the sensor and parse through the data, finding the
 *	first six objects that are larger than 2 degrees
 *	@param sweepDataPtr A 2D array that will store the sensor information
 *	@param finds 		An array of structs which contain information about the objects found
 *	@param objectCount 	The number of objects found
 */
void read_turret(volatile float (*sweepDataPtr)[2][91],volatile  struct block (*finds)[6],volatile  int *objectCount){
    ///record data
    sweep_servo(sweepDataPtr);
    ///reset the previous objectCount
    *objectCount = 0;
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
        if((*sweepDataPtr)[0][i] < 70 && (*sweepDataPtr)[1][i] < 50 && looking == 0
                && (*sweepDataPtr)[0][i+1] < 100 && (*sweepDataPtr)[1][i+1])
        {
            looking = 1;
        }
        ///if the end of an object is found
        if(((*sweepDataPtr)[0][i] >= 70 || (*sweepDataPtr)[1][i] >= 50) && looking == 1)
        {
            ///set boolean variable indicating an object found to 0
            looking = 0;
            ///record distance
            (*finds)[*objectCount].distance = minDist;
            ///compute center of object
            (*finds)[*objectCount].center = i*2-currentWidth;
            ///compute width of object using arc length formula
            (*finds)[*objectCount].width = currentWidth * (6.28 / 180) * minDist;
            ///compute x coordinate of object
            (*finds)[*objectCount].x = ((*finds)[*objectCount].distance + ((*finds)[*objectCount].width / 2)) * cos((*finds)[*objectCount].center * 3.14 / 180);
            ///compute y coordinate of object
            (*finds)[*objectCount].y = ((*finds)[*objectCount].distance + ((*finds)[*objectCount].width / 2)) * sin((*finds)[*objectCount].center * 3.14 / 180);
            ///iterate to the next block in the block array
            *objectCount = *objectCount + 1;
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
            if((*sweepDataPtr)[1][i] < minDist)
            {
                minDist = (*sweepDataPtr)[1][i];
            }
        }
        ///if the block array is full
        if(*objectCount == 6)
        {
            break;
        }
    }
}
